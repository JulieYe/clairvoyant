data.folder <- "~/Projects/futures_trade/data/FeatureData"

load.data <- function(data.folder, subsamplefreq=10)
{
  filenames <- dir(data.folder)
  idx <- grep('feature', filenames)
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

save(dat, file="data.RData")

xind <- c(grep("bookPressure", colnames(dat)), grep("BookPressure", colnames(dat)), grep("ShortTermReturn", colnames(dat)), grep("TradeDeviation", colnames(dat)), grep("SignedVolume", colnames(dat)),
          grep("Vwap", colnames(dat)), grep("^BookSize", colnames(dat)))
x <- dat[,xind]
y <- dat[,"MidPoint_2000ms"] - dat[,"MidPoint"]

sd.summary <- apply(as.matrix(x), 2, sd)
sd.summary <- data.frame(signal = colnames(x), sd.5 = sd.summary * 5)

cap.data <- function(x, n.sd=5)
{
  for(ii in 1:ncol(x)) {
    x.sd <- sd(x[,ii])
    x[which(x[,ii] > 5 * x.sd), ii] <- 5 * x.sd
    x[which(x[,ii] < -5 * x.sd), ii] <- -5 * x.sd
  }
  return(x)
}

newx <- cap.data(x, n.sd=5)

rdat <- data.frame(y, newx)


in.sample.idx <- 1:round(nrow(rdat) * 0.7)
out.sample.idx <- round(nrow(rdat) * 0.7):nrow(rdat)


mm <- lm(y~0+., data=rdat[in.sample.idx,])
newmm <- step(mm)

print(summary(newmm))

y.outsample.hat <- predict(newmm, newdata=rdat[out.sample.idx,])
y.outsample <- y[out.sample.idx]

print(cor(y.outsample.hat, y.outsample))

print(1 - sum( (y.outsample - y.outsample.hat) ^ 2) / sum(y.outsample ^ 2))


      
