args <- commandArgs(T)
begin.date <- as.numeric(args[[1]])
end.date <- as.numeric(args[[2]])

smoothing.num.seconds <- 10  # 0, 2, 5, 10, 20, 30, 40, 50, 60, 90, 120
edge.list <- seq(0.2, 2.0, 0.2)
buffer.list <- seq(0.2, 1.0, 0.2)
fd <- file("smms.model.parameter.selection.IF.csv", "w")
writeLines(paste("edge", "buffer", "sample_size", "revert_size", "revert_percentage", "mean_revert_time", "mean_max_edge", "mean_med_edge", "mean_min_edge", sep=","), con=fd, sep="\n")

if (smoothing.num.seconds == 0) { 
  mid0.name <- "ImpliedSpreadPrice.Mid."
} else { 
  mid0.name <- sprintf("ImpliedSpreadPriceSmooth.Mid.%s.Second.", smoothing.num.seconds)
}
bid1.name <- "ImpliedSpreadPrice.Bid._500ms"
ask1.name <- "ImpliedSpreadPrice.Ask._500ms"

load.data <- function(data.folder, ticker.head)
{
  filenames <- dir(data.folder, ticker.head)
  idx <- grep(sprintf('features..........%s.....%s.....csv', ticker.head, ticker.head), filenames)
  filenames <- filenames[idx]
  dat <- NULL
  for (fn in filenames) {
    if (as.numeric(strsplit(fn, '\\.')[[1]][2]) >= begin.date && 
        as.numeric(strsplit(fn, '\\.')[[1]][2]) <= end.date) {
      full.fn <- paste(data.folder, fn, sep="/")
      print(sprintf("reading %s", full.fn))
      tmp <- read.csv(full.fn)
      mid0.idx <- grep(mid0.name, colnames(tmp))
      bid1.idx <- grep(bid1.name, colnames(tmp))
      ask1.idx <- grep(ask1.name, colnames(tmp))
      if (length(mid0.idx) == 0 || length(bid1.idx) == 0 || length(ask1.idx) == 0) {
        next
      }
      tmp <- tmp[(tmp[,bid1.idx]-tmp[,mid0.idx])*(tmp[,ask1.idx]-tmp[,mid0.idx])>=0,]
      dat <- rbind(dat, tmp)
    }
  }
  return(dat)
}

process.line.bid <- function(row.data, mid.idxs, mid0.idx, buff.curr)
{
  ret <- Inf
  ret.edge <- NULL
  for (i in mid.idxs) {
    ret.this <- row.data[i] - row.data[mid0.idx]
    if (ret.this >= -buff.curr) {
      ret <- min(ret, mid.idxs)
    }
    ret.edge <- c(ret.edge, ret.this)
  }
  return(c(ret, max(ret.edge), median(ret.edge), min(ret.edge)))
}

process.line.ask <- function(row.data, mid.idxs, mid0.idx, buff.curr)
{
  ret <- Inf
  ret.edge <- NULL
  for (i in mid.idxs) {
    ret.this <- row.data[mid0.idx] - row.data[i]
    if (ret.this >= -buff.curr) {
      ret <- min(ret, mid.idxs)
    }
    ret.edge <- c(ret.edge, ret.this)
  }
  return(c(ret, max(ret.edge), median(ret.edge), min(ret.edge)))
}

data <- load.data("~/Documents/futures_trade/data/CFFEX", "IF")
mid0.idx <- grep(mid0.name, colnames(data))
bid1.idx <- grep(bid1.name, colnames(data))
ask1.idx <- grep(ask1.name, colnames(data))

for (edge in edge.list) { 
for (buffer in buffer.list) { 
data.bid <- data[data[,ask1.idx] <= data[,mid0.idx] - (edge + buffer),]
data.ask <- data[data[,bid1.idx] >= data[,mid0.idx] + (edge + buffer),]
#print(dim(data))
#print(dim(data.bid))
#print(dim(data.ask))

samp.size <- dim(data.bid)[1] + dim(data.ask)[1]
revt.size <- 0
revt.time <- NULL
revt.max.edge <- NULL
revt.med.edge <- NULL
revt.min.edge <- NULL
col.names <- colnames(data.bid)
mid.idxs <- NULL
for (i in c(1,2,3,4,5,10,20,30,40,50,60)) {
  mid.idxs <- c(mid.idxs, grep(sprintf("ImpliedSpreadPrice.Mid._%s000ms", i), col.names))
}
if (dim(data.bid)[1] > 0) {
  for (i in 1:(dim(data.bid)[1])) {
    ret = process.line.bid(as.numeric(data.bid[i,]), mid.idxs, mid0.idx, buffer)
    revt.max.edge <- c(revt.max.edge, ret[2])
    revt.med.edge <- c(revt.med.edge, ret[3])
    revt.min.edge <- c(revt.min.edge, ret[4])
    #print(data.bid[i,])
    if (ret[1] != Inf) {
      revt.size <- revt.size + 1
      revt.time <- c(revt.time, ret)
    }
  }
}
if (dim(data.ask)[1] > 0) {
  for (i in 1:(dim(data.ask)[1])) {
    ret = process.line.ask(as.numeric(data.ask[i,]), mid.idxs, mid0.idx, buffer)
    revt.max.edge <- c(revt.max.edge, ret[2])
    revt.med.edge <- c(revt.med.edge, ret[3])
    revt.min.edge <- c(revt.min.edge, ret[4])
    #print(data.ask[i,])
    if (ret[1] != Inf) {
      revt.size <- revt.size + 1
      revt.time <- c(revt.time, ret)
    }
  }
}
print(c(round(edge,2), round(buffer,2), samp.size, revt.size, round(revt.size/samp.size*100), round(mean(revt.time)), round(mean(revt.max.edge),2), round(mean(revt.med.edge),2), round(mean(revt.min.edge),2)))
writeLines(paste(round(edge,2), round(buffer,2), samp.size, revt.size, round(revt.size/samp.size*100), round(mean(revt.time)), round(mean(revt.max.edge),2), round(mean(revt.med.edge),2), round(mean(revt.min.edge),2), sep=","), con=fd, sep="\n")
}
}