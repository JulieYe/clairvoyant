commission <- list()
commission['IF'] <- 23
commission['ru'] <- 13.8
commission['rb'] <- 2.6
commission['cu'] <- 17.3

slippage <- list()
slippage['IF'] <- 30

args <- commandArgs(T)

data.file <- args[[1]]
pdf.file <- args[[2]]
ticker.head <- args[[3]]
strategy.name <- args[[4]]

data <- read.csv(data.file, stringsAsFactors=FALSE)
data <- data[data[,1]=='StrategyStatus',]
data <- data.frame(data, stringsAsFactors=FALSE)
print(dim(data))

pnl <- as.numeric(data[seq(1, dim(data)[1], length.out = 1000),4])
volume <- as.numeric(data[seq(1, dim(data)[1], length.out = 1000),7])
pnl <- pnl - volume * slippage[[ticker.head]]
pnl.after.commission <- pnl - volume * commission[[ticker.head]]

pdf(pdf.file)
plot(pnl)
plot(volume)
plot(pnl.after.commission)
dev.off()