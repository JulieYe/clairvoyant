library("foba");
source("generateHppCpp.R")

#data.folder <- "~/Documents/futures_trade/data/CFFE";

default.target.folder <- "../src/"
default.template.cpp <- "Cpp.template"
default.template.hpp <- "Hpp.template"

args <- commandArgs(T);

data.folder <- args[[1]];
pdf.file <- args[[2]];
ticker.head <- args[[3]];
num.features <- as.numeric(args[[4]]);

#data.folder <- "../data/RBData"
#pdf.file <- "modelselection.rb.pdf"
#ticker.head <- "rb"

load.data <- function(data.folder, subsamplefreq=5)
{
  filenames <- dir(data.folder);
  idx <- grep(sprintf('features.201......%s', ticker.head), filenames);
  filenames <- filenames[idx];
  dat <- NULL;
  for(fn in filenames) {
    full.fn <- paste(data.folder, fn, sep="/");
    print(sprintf("reading %s", full.fn));
    tmp <- read.csv(full.fn);
    sub.idx <- seq(subsamplefreq, nrow(tmp), by=subsamplefreq);
    tmp <- tmp[sub.idx,];
    print(dim(tmp));
    dat <- rbind(dat, tmp);
  }
  return(dat);
};

dat <- load.data(data.folder);
print(dim(dat));

#xind <- c(grep("^bookPressure", colnames(dat)), grep("^BookPressure", colnames(dat)), grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)), grep("^WindowedBreakThrough", colnames(dat)), grep("^TradeBreakOut", colnames(dat)), grep("^EMAWindowedBreakthrough", colnames(dat)));
#xind <- c(grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)), grep("^WindowedBreakThrough", colnames(dat)), grep("^TradeBreakOut", colnames(dat)), grep("^EMAWindowedBreakthrough", colnames(dat)));
#xind <- c(grep("^bookPressure", colnames(dat)), grep("^BookPressure", colnames(dat)), grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)));
xind <- c(grep("^ShortTermReturn", colnames(dat)), grep("^TradeDeviation", colnames(dat)), grep("^SignedVolume", colnames(dat)), grep("^Vwap", colnames(dat)), grep("^BookSize", colnames(dat)));
x <- dat[,xind];
y <- dat[,"MidPoint_2000ms"] - dat[,"MidPoint"];

sd.y <- sd(y)
y[y > 5 * sd.y] <- 5 * sd.y
y[y < -5 * sd.y] <- -5 * sd.y

count.outlier <- function(x)
{
  sd5 <- 5 * sd(x);
  sum(abs(x)>sd5);
};

apply(as.matrix(x), 2, count.outlier);

save.sd <- function(x, n.sd=5)
{
  ret = rep(0,ncol(x));
  for(ii in 1:ncol(x)) {
    ret[ii] <- sd(x[,ii]) * n.sd;
  }
  return(ret);
};

cap.data <- function(x, n.sd=5)
{
  for(ii in 1:ncol(x)) {
    x.sd <- sd(x[,ii]);
    x[which(x[,ii] > n.sd * x.sd), ii] <- n.sd * x.sd;
    x[which(x[,ii] < -n.sd * x.sd), ii] <- -n.sd * x.sd;
  }
  return(x);
};

f.sd <- save.sd(x);
newx <- cap.data(x, n.sd=5);

rand.idx <- sample(1:length(y),length(y))
y <- y[rand.idx]
newx <- newx[rand.idx,]
rdat <- data.frame(y, newx);

in.sample.idx <- 1:round(nrow(rdat) * 0.7);
out.sample.idx <- round(nrow(rdat) * 0.7):nrow(rdat);

model <- foba(newx[in.sample.idx,], y[in.sample.idx], intercept=FALSE);
print(model$path)

num.features <- min(num.features, ncol(newx))
cor.tr.sparse <- rep(0, num.features);
cor.ts.sparse <- rep(0, num.features);
r2.tr.sparse <- rep(0, num.features);
r2.ts.sparse <- rep(0, num.features);
for(ii in 1:num.features) {
  coef <- predict(model, k=ii, type="coef");
  y.tr.hat <- c(as.matrix(newx[in.sample.idx,]) %*% as.matrix(coef$coef, ncol=1));
  y.tr <- y[in.sample.idx];
  y.ts.hat <- c(as.matrix(newx[out.sample.idx,]) %*% as.matrix(coef$coef, ncol=1));
  y.ts <- y[out.sample.idx];
  cor.tr.sparse[ii] <- cor(y.tr.hat, y.tr);
  cor.ts.sparse[ii] <- cor(y.ts.hat, y.ts);
  r2.tr.sparse[ii] <- 1 - sum( (y.tr - y.tr.hat) ^ 2) / sum(y.tr ^ 2);
  r2.ts.sparse[ii] <- 1 - sum( (y.ts - y.ts.hat) ^ 2) / sum(y.ts ^ 2);
};

#pdf('model_selection.IF.pdf');
pdf(pdf.file);
cor.min <- min(c(cor.tr.sparse, cor.ts.sparse));
cor.max <- max(c(cor.tr.sparse, cor.ts.sparse));
plot(cor.tr.sparse, col="blue", ylim=c(cor.min,cor.max), xlab="sparsity", ylab="correlation", main=paste("Training:", which.max(cor.tr.sparse), ", Testing:", which.max(cor.ts.sparse)));
points(cor.ts.sparse, col="red");
r2.min <- min(c(r2.tr.sparse, r2.ts.sparse));
r2.max <- max(c(r2.tr.sparse, r2.ts.sparse));
plot(r2.tr.sparse, col="blue", ylim=c(r2.min,r2.max), xlab="sparsity", ylab="R2", main=paste("Training:", which.max(r2.tr.sparse), ", Testing:",  which.max(r2.ts.sparse)));
points(r2.ts.sparse, col="red");
dev.off();

num.features <- which.max(r2.ts.sparse);
coef <- predict(model, k=num.features, type="coef");
vars <- colnames(newx)[coef$coef!=0];
print(vars);
print(f.sd[coef$coef!=0]);

x.select <- newx[,coef$coef!=0];
#compare with foba coef with same sparsity for sanity check
#foba.select = predict(model, k=num.features, type="coef")$coef
#print(foba.select[foba.select!=0])
#model.select <- lm(y[in.sample.idx]~0+., data=x.select[in.sample.idx,])
#lm.select <- as.numeric(model.select$coef)
#print(lm.select)

model.select <- lm(y~0+., data=x.select);
print(as.numeric(model.select$coef));

signal.str <- generate.signal(vars)
signal.register <- generate.register(vars)
beta.str <- generate.beta(as.numeric(model.select$coef))
bound.str <- generate.bound(f.sd[coef$coef!=0])

write.cpp(ticker.head, default.template.cpp, default.target.folder)
write.hpp(ticker.head, default.template.hpp, default.target.folder,
          signal.str, signal.register, beta.str, bound.str)

q('no');
