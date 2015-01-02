commission <- list()
commission['IF'] <- 19

args <- commandArgs(T)

data.folder <- args[[1]]
pdf.file <- args[[2]]
ticker.head <- args[[3]]

load.data <- function(data.folder)
{
  filenames <- dir(data.folder)
  idx <- grep(sprintf('201......%s.....log', ticker.head), filenames)
  filenames <- filenames[idx]
  dat <- NULL
  for(fn in filenames) {
    full.fn <- paste(data.folder, fn, sep="/")
    print(sprintf("reading %s", full.fn))
    tmp <- read.csv(full.fn, stringsAsFactors=FALSE)
    tmp <- tmp[tmp[,1]=='StrategyStatus',]
    nrow <- dim(tmp)[1]
    dat <- rbind(dat, tmp[nrow,c(2,4,7)])
  }
  return(dat)
}

data <- load.data(data.folder)

dates <- as.Date(data[,1])
pnl <- as.numeric(data[,2])
volume <- as.numeric(data[,3])
pnl.after.commission <- pnl - volume * commission[[ticker.head]]

cum.pnl.after.commission <- cumsum(pnl.after.commission)

initial <- 200000
equity.curve <- cum.pnl.after.commission + initial

num.days <- length(equity.curve)
first <- initial
last <- equity.curve[num.days]
return.period <- round((last-first) / first * 100, 2)
return.annual <- round(return.period / num.days * 252, 2)

return.after.commission <- round(pnl.after.commission / initial * 100, 2)
return.min <- min(return.after.commission)
return.max <- max(return.after.commission)
date.min <- dates[which.min(return.after.commission)]
date.max <- dates[which.max(return.after.commission)]

pdf(pdf.file)
plot(dates, equity.curve, type="l", col="blue", xlab="Date", ylab="Equity Curve", main=paste("Period Return (%):", return.period, "Annual Return (%):", return.annual))
barplot(return.after.commission, names.arg=dates, xlab="Date", ylab="Daily Return (%)", main=paste("Max Daily Return:",return.max,date.max,"Min Daily Return",return.min,date.min))
dev.off()