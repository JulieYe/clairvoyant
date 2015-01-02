args <- commandArgs(T);

data.folder <- args[[1]];
pdf.file <- args[[2]];
ticker.head <- args[[3]];

load.data <- function(data.folder, ticker.head)
{
  filenames <- dir(data.folder);
  idx <- grep(sprintf('^201......%s....', ticker.head), filenames);
  filenames <- filenames[idx];
  print(length(filenames))
  dat <- NULL;
  for(fn in filenames) {
    full.fn <- paste(data.folder, fn, sep="/");
    print(sprintf("reading %s", full.fn));
    tmp <- read.csv(full.fn);
    last.am <- substr(tmp[,1],12,19) == "11:29:00"
    last.fm <- substr(tmp[,1],12,19) == "15:14:00"
    if (sum(last.am) > 0 && sum(last.fm) > 0) {
      volume.am <- tmp[last.am,3][1]
      volume.fm <- tmp[last.fm,3][1] - volume.am
    }
    dat <- rbind(dat, c(volume.am, volume.fm));
  }
  return(dat);
};

dat <- load.data(data.folder, ticker.head);
print(dim(dat));

pdf(pdf.file);
hist(dat[,1] / 100000, breaks=seq(0, 10, 0.5), freq=FALSE, col=rgb(0,0,1,1/4), xlab="Volume (10k)", main="AM/FM Volume Analysis")
hist(dat[,2] / 100000, breaks=seq(0, 10, 0.5), freq=FALSE, col=rgb(1,0,0,1/4), add=TRUE)
dev.off();

q("no")