library("foba")
source("generateHppCpp.R")

#options(width=600)

data.folder <- "~/Documents/futures_trade/data/CFFEX"
ticker.head <- "IF"
currency.per.point <- 300
commission.per.lot <- 19.5
num.fold <- 10
signal.thres.begin <- 0.1
signal.thres.end <- 0.6
signal.thres.step <- 0.01
vol.thres.begin <- 0.1
vol.thres.end <- 0.01
vol.thres.step <- 0.5
write.file <- 0
output.file.name <- "test.csv"

default.target.folder <- "../src/"
default.template.cpp <- "Cpp.template"
default.template.hpp <- "Hpp.template"

args <- commandArgs(T)

data.folder <- args[[1]]
ticker.head <- args[[2]]
currency.per.point <- as.numeric(args[[3]])
commission.per.lot <- as.numeric(args[[4]])
num.fold <- as.numeric(args[[5]])
signal.thres.begin <- as.numeric(args[[6]])
signal.thres.end <- as.numeric(args[[7]])
signal.thres.step <- as.numeric(args[[8]])
vol.thres.begin <- as.numeric(args[[9]])
vol.thres.end <- as.numeric(args[[10]])
vol.thres.step <- as.numeric(args[[11]])
write.file <- as.numeric(args[[12]])
output.file.name <- args[[13]]

load.data <- function(data.folder, subsamplefreq=10)
{
  filenames <- dir(data.folder)
  idx <- grep(sprintf('features..........%s.....csv', ticker.head), filenames)
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

optimal.sparsity <- function(data.matrix)
{
  #mean.sparsity <- apply(data.matrix, 2, mean)
  #sd.sparsity <- apply(data.matrix, 2, sd)
  #threshold.sparsity <- max(mean.sparsity) - sd.sparsity[which.max(mean.sparsity)]
  #for (i in 1:dim(data.matrix)[2]) {
    #if (mean.sparsity[i] >= threshold.sparsity) {
      #return(i)
    #}
  #}
  return(which.max(apply(data.matrix, 2, mean)))
}

dat <- load.data(data.folder, 9)
print(dim(dat))

#xind <- c(grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)), grep("^bookPressure", colnames(dat)))
xind <- c(grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)), grep("^SweepStrong", colnames(dat)))
x <- dat[,xind]
y <- dat[,"MidPoint_2000ms"] - dat[,"MidPoint"]
yy <- dat[,"MidPoint_5000ms"] - dat[,"MidPoint"]
yyy <- dat[,"MidPoint_10000ms"] - dat[,"MidPoint"]
z <- dat[,grep("^HighFrequencyVolatility.60.2.Second", colnames(dat))]

sd.y <- sd(y)
print(c(min(y), max(y)))
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
  for (i in 1:ncol(x)) {
    ret[i] <- sd(x[,i]) * n.sd
  }
  return(ret)
}

cap.data <- function(x, n.sd=5)
{
  for (i in 1:ncol(x)) {
    x.sd <- sd(x[,i])
    if (length(which(x[,i] > n.sd * x.sd)) > 0) {
      x[which(x[,i] > n.sd * x.sd), i] <- n.sd * x.sd
    }
    if (length(which(x[,i] < -n.sd * x.sd)) > 0) {
      x[which(x[,i] < -n.sd * x.sd), i] <- -n.sd * x.sd
    }
  }
  return(x)
}

f.sd <- save.sd(x)
newx <- cap.data(x)

num.features <- ncol(newx)
cat(paste("Number of total features:", num.features, "\n"))
num.samp.global <- length(y)

seq.global <- sample(1:num.samp.global)
fold.size <- round(num.samp.global / num.fold)
r2.global <- matrix(NaN, nrow=num.fold, ncol=num.features)

for (i in 1:num.fold) {
  cat(paste("Working on fold:", i, "\n"))

  idx.test.begin <- (i - 1) * fold.size + 1
  idx.test.end <- min(idx.test.begin + fold.size - 1, num.samp.global)
  seq.local.ts <- seq.global[idx.test.begin:idx.test.end]
  seq.local.tr <- seq.global[is.na(pmatch(seq.global, seq.local.ts))]

  Y.tr <- y[seq.local.tr]
  X.tr <- newx[seq.local.tr,]
  Y.ts <- y[seq.local.ts]
  X.ts <- newx[seq.local.ts,]

  model.foba.local <- foba(X.tr, Y.tr, intercept=FALSE)
  for (j in 1:num.features) {
    coef <- predict(model.foba.local, k=j, type='coef')
    Y.ts.hat <- c(as.matrix(X.ts) %*% as.matrix(coef$coef, ncol=1))
    r2.global[i, j] <- 1 - (1 - sum(Y.ts.hat ^ 2) / (sum(Y.ts.hat ^2) + sum((Y.ts-Y.ts.hat)^2))) * length(Y.ts) / (length(Y.ts) - j)
  }
}

cat(paste("\n", num.fold, "fold out-of-sample adjusted R2 matrix:", "\n"))
print(r2.global)
sparsity <- optimal.sparsity(r2.global)
cat(paste("\n", "sparsity:", sparsity, "with average out-of-sample adjusted R2:", mean(r2.global[,sparsity]), "and sd of R2:", sd(r2.global[,sparsity]), "\n\n"))

model.foba.global <- foba(newx, y, intercept=F)
coef <- predict(model.foba.global, k=sparsity, type='coef')
print(coef)
print(coef$coef)

fit <- c(as.matrix(newx) %*% as.matrix(coef$coef, ncol=1))
cat(paste(mean(fit[fit>0]), sd(fit[fit>0]), mean(fit[fit<0]), sd(fit[fit<0]), "\n"))
cat(paste(mean(y[fit>0]), sd(y[fit>0]), mean(y[fit<0]), sd(y[fit<0]), "\n"))
cat("\n")
fd <- file(output.file.name, "w")
writeLines(paste("pred_horizon_s", "signal_thres", "vol_thres", "point_per_round", "pnl_total", "pnl_per_round", "num_rounds", "point_avg_c_up", "point_avg_up_c_up", "percentage_up_c_up", "point_avg_down_c_up", "percentage_down_c_up", "num_rounds_up", "point_avg_c_down", "point_avg_down_c_down", "percentage_down_c_down", "point_avg_up_c_down", "percentage_up_c_down", "num_rounds_down", sep=","), con=fd, sep="\n")
signal.thres <- signal.thres.begin
while (signal.thres <= signal.thres.end) {
  vol.thres <- vol.thres.begin
  while (vol.thres <= vol.thres.end) {
    pred.up <- y[z > vol.thres & fit > signal.thres]
    predd.up <- yy[z > vol.thres & fit > signal.thres]
    preddd.up <- yyy[z > vol.thres & fit > signal.thres]
    pred.down <- y[z > vol.thres & fit < -signal.thres]
    predd.down <- yy[z > vol.thres & fit < -signal.thres]
    preddd.down <- yyy[z > vol.thres & fit < -signal.thres]
    # print(c(signal.thres, vol.thres, min(pred.up), max(pred.up), min(pred.down), max(pred.down)))
    pnl.up.total <- sum(pred.up) * currency.per.point - 2 * commission.per.lot * length(pred.up)
    pnll.up.total <- sum(predd.up) * currency.per.point - 2 * commission.per.lot * length(predd.up)
    pnlll.up.total <- sum(preddd.up) * currency.per.point - 2 * commission.per.lot * length(preddd.up)
    pnl.down.total <- -sum(pred.down) * currency.per.point - 2 * commission.per.lot * length(pred.down)
    pnll.down.total <- -sum(predd.down) * currency.per.point - 2 * commission.per.lot * length(predd.down)
    pnlll.down.total <- -sum(preddd.down) * currency.per.point - 2 * commission.per.lot * length(preddd.down)
    pnl.total <- pnl.up.total + pnl.down.total
    pnll.total <- pnll.up.total + pnll.down.total
    pnlll.total <- pnlll.up.total + pnlll.down.total
    pnl.avg <- pnl.total / (length(pred.up) + length(pred.down))
    pnll.avg <- pnll.total / (length(predd.up) + length(predd.down))
    pnlll.avg <- pnlll.total / (length(preddd.up) + length(preddd.down))
    point.per.round <- (sum(pred.up) - sum(pred.down)) / (length(pred.up) + length(pred.down))
    pointt.per.round <- (sum(predd.up) - sum(predd.down)) / (length(predd.up) + length(predd.down))
    pointtt.per.round <- (sum(preddd.up) - sum(preddd.down)) / (length(preddd.up) + length(preddd.down))
    writeLines(paste(2, round(signal.thres, 4), round(vol.thres, 4), round(point.per.round, 4), round(pnl.total), round(pnl.avg), length(pred.up) + length(pred.down), round(mean(pred.up),4), round(mean(pred.up[pred.up>0]),4), round(100*sum(pred.up>0)/length(pred.up),4), round(mean(pred.up[pred.up<0]),4), round(100*sum(pred.up<0)/length(pred.up),4), length(pred.up), round(mean(pred.down),4), round(mean(pred.down[pred.down<0]),4), round(100*sum(pred.down<0)/length(pred.down),4), round(mean(pred.down[pred.down>0]),4), round(100*sum(pred.down>0)/length(pred.down),4), length(pred.down), sep=","), con=fd, sep="\n")
    writeLines(paste(5, round(signal.thres, 4), round(vol.thres, 4), round(pointt.per.round, 4), round(pnll.total), round(pnll.avg), length(predd.up) + length(predd.down), round(mean(predd.up),4), round(mean(predd.up[predd.up>0]),4), round(100*sum(predd.up>0)/length(predd.up),4), round(mean(predd.up[predd.up<0]),4), round(100*sum(predd.up<0)/length(predd.up),4), length(predd.up), round(mean(predd.down),4), round(mean(predd.down[predd.down<0]),4), round(100*sum(predd.down<0)/length(predd.down),4), round(mean(predd.down[predd.down>0]),4), round(100*sum(predd.down>0)/length(predd.down),4), length(predd.down), sep=","), con=fd, sep="\n")
    writeLines(paste(10, round(signal.thres, 4), round(vol.thres, 4), round(pointtt.per.round, 4), round(pnlll.total), round(pnlll.avg), length(preddd.up) + length(preddd.down), round(mean(preddd.up),4), round(mean(preddd.up[preddd.up>0]),4), round(100*sum(preddd.up>0)/length(preddd.up),4), round(mean(preddd.up[preddd.up<0]),4), round(100*sum(preddd.up<0)/length(preddd.up),4), length(preddd.up), round(mean(preddd.down),4), round(mean(preddd.down[preddd.down<0]),4), round(100*sum(preddd.down<0)/length(preddd.down),4), round(mean(preddd.down[preddd.down>0]),4), round(100*sum(preddd.down>0)/length(preddd.down),4), length(preddd.down), sep=","), con=fd, sep="\n")
    vol.thres <- vol.thres + vol.thres.step
    }
    signal.thres <- signal.thres + signal.thres.step
}
close(fd)

if (write.file > 0) {
  vars <- colnames(newx)[coef$coef != 0]
  signal.str <- generate.signal(vars)
  signal.register <- generate.register(vars)
  beta.str <- generate.beta(coef$coef[coef$coef != 0])
  bound.str <- generate.bound(f.sd[coef$coef != 0])
  write.cpp(ticker.head, default.template.cpp, default.target.folder)
  write.hpp(ticker.head, default.template.hpp, default.target.folder,
            signal.str, signal.register, beta.str, bound.str)
}