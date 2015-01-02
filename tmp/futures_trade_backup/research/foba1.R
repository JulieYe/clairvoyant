library("foba")
source("generateHppCpp.R")

#data.folder <- "~/Documents/futures_trade/data/CFFE"

default.target.folder <- "../src/"
default.template.cpp <- "Cpp.template"
default.template.hpp <- "Hpp.template"

args <- commandArgs(T)

data.folder <- args[[1]]
pdf.file <- args[[2]]
ticker.head <- args[[3]]
num.features <- as.numeric(args[[4]])
num.randomization <- as.numeric(args[[5]])

#data.folder <- "../data/RBData"
#pdf.file <- "modelselection.rb.pdf"
#ticker.head <- "rb"

load.data <- function(data.folder, subsamplefreq=20)
{
  filenames <- dir(data.folder)
  idx <- grep(sprintf('features.201......%s', ticker.head), filenames)
  filenames <- filenames[idx]
  dat <- NULL
  for(fn in filenames) {
    full.fn <- paste(data.folder, fn, sep="/")
    print(sprintf("reading %s", full.fn))
    tmp <- read.csv(full.fn)
    sub.idx <- seq(subsamplefreq, nrow(tmp), by=subsamplefreq)
    tmp <- tmp[sub.idx,]
    print(dim(tmp))
    dat <- rbind(dat, tmp)
  }
  return(dat)
}

dat <- load.data(data.folder)
print(dim(dat))

#xind <- c(grep("^bookPressure", colnames(dat)), grep("^BookPressure", colnames(dat)), grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)), grep("^WindowedBreakThrough", colnames(dat)), grep("^TradeBreakOut", colnames(dat)), grep("^EMAWindowedBreakthrough", colnames(dat)))
#xind <- c(grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)), grep("^WindowedBreakThrough", colnames(dat)), grep("^TradeBreakOut", colnames(dat)), grep("^EMAWindowedBreakthrough", colnames(dat)))
#xind <- c(grep("^bookPressure", colnames(dat)), grep("^BookPressure", colnames(dat)), grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)))
xind <- c(grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)))
x <- dat[,xind]
y <- dat[,"MidPoint_2000ms"] - dat[,"MidPoint"]

sd.y <- sd(y)
y[y > 5 * sd.y] <- 5 * sd.y
y[y < -5 * sd.y] <- -5 * sd.y

count.outlier <- function(x)
{
  sd5 <- 5 * sd(x)
  sum(abs(x)>sd5)
}

apply(as.matrix(x), 2, count.outlier)

save.sd <- function(x, n.sd=5)
{
  ret = rep(0,ncol(x))
  for(ii in 1:ncol(x)) {
    ret[ii] <- sd(x[,ii]) * n.sd
  }
  return(ret)
}

cap.data <- function(x, n.sd=5)
{
  for(ii in 1:ncol(x)) {
    x.sd <- sd(x[,ii])
    if(length(which(x[,ii] > n.sd * x.sd))>0) {
      x[which(x[,ii] > n.sd * x.sd), ii] <- n.sd * x.sd
    }
    if(length(which(x[,ii] < -n.sd * x.sd))>0) {
      x[which(x[,ii] < -n.sd * x.sd), ii] <- -n.sd * x.sd
    }
  }
  return(x)
}

f.sd <- save.sd(x)
newx <- cap.data(x)

count.selected.features <- rep(0, ncol(newx))
print("count.selected.features at start:")
print(count.selected.features)

for(kkk in 1:num.randomization) {
  print("working on randomization number:")
  print(kkk)
  rand.idx <- sample(1:length(y),length(y))
  y <- y[rand.idx]
  newx <- newx[rand.idx,]
  rdat <- data.frame(y, newx)
  in.sample.idx <- 1:round(nrow(rdat) * 0.7)
  out.sample.idx <- round(nrow(rdat) * 0.7):nrow(rdat)
  model <- foba(newx[in.sample.idx,], y[in.sample.idx], intercept=FALSE)
  #print(model$path)
  num.features <- min(num.features, ncol(newx))
  r2.ts.sparse <- rep(0, num.features)
  for(ii in 1:num.features) {
    coef <- predict(model, k=ii, type="coef")
    y.ts.hat <- c(as.matrix(newx[out.sample.idx,]) %*% as.matrix(coef$coef, ncol=1))
    y.ts <- y[out.sample.idx]
    r2.ts.sparse[ii] <- 1 - sum( (y.ts - y.ts.hat) ^ 2) / sum(y.ts ^ 2)
  }
  coef <- predict(model, k=which.max(r2.ts.sparse), type="coef")
  print("max R2:")
  print(max(r2.ts.sparse))
  for(iii in 1:length(coef$coef)) {
    if(coef$coef[iii]!=0) {
      count.selected.features[iii] <- count.selected.features[iii] + 1
    }
  }
}

print("count.selected.features at end:")
print(count.selected.features)

print("total number of features selected:")
print(sum(count.selected.features>0.75*num.randomization))
x.select <- newx[,count.selected.features>0.75*num.randomization]
model.select <- lm(y~0+., data=x.select)
print(summary(model.select))

vars <- colnames(newx)[count.selected.features>0.75*num.randomization]
signal.str <- generate.signal(vars)
signal.register <- generate.register(vars)
beta.str <- generate.beta(as.numeric(model.select$coef))
bound.str <- generate.bound(f.sd[count.selected.features>0.75*num.randomization])

write.cpp(ticker.head, default.template.cpp, default.target.folder)
write.hpp(ticker.head, default.template.hpp, default.target.folder,
          signal.str, signal.register, beta.str, bound.str)

q('no')

