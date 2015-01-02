args <- commandArgs(T)

filename <- args[[1]]
pdf.file <- args[[2]]
stats.file <- args[[3]]
threshold <- as.numeric(args[[4]])

fd <- file(stats.file, "w")
writeLines(paste("OrderType", "OrderRef", "RspTime", "Time", sep=","), con=fd, sep="\n")

lines <- readLines(filename)

latency.data <- data.frame()
for (i in 1:length(lines)) {
  fields <- strsplit(lines[i],",")[[1]]
  if (fields[1] == "LimitOrderMessage") {
    att <- TRUE
    order.num <- strsplit(fields[11], "=")[[1]][2]
    ts.string <- strsplit(fields[7], "=")[[1]][2]
    time.in <- as.numeric(strsplit(fields[8], "=")[[1]][2])
    for (j in (i+1):length(lines)) {
      fields <- strsplit(lines[j],",")[[1]]
      if (fields[1] == "AcknowledgedOrderMessage" && 
          strsplit(fields[9], "=")[[1]][2] == order.num && 
	  strsplit(fields[2], "=")[[1]][2] == "0") {
        time.out <- as.numeric(strsplit(fields[6], "=")[[1]][2])
	print(ts.string)
	print(c(0, as.numeric(order.num), round((time.out-time.in)/10^6)))
	latency.data <- rbind(latency.data, data.frame(OrderType=0, OrderRef=as.numeric(order.num), RspTime=(time.out-time.in)/10^6, Time=ts.string))
	att <- FALSE
	break
      } else if (is.element(fields[1], c("RejectLimitOrderMessage", "FilledOrderMessage", "CanceledLimitOrderMessage")) && 
                 strsplit(fields[11], "=")[[1]][2] == order.num) {
          time.out <- as.numeric(strsplit(fields[8], "=")[[1]][2])
	  print(ts.string)
	  print(c(0, as.numeric(order.num), round((time.out-time.in)/10^6)))
	  latency.data <- rbind(latency.data, data.frame(OrderType=0, OrderRef=as.numeric(order.num), RspTime=(time.out-time.in)/10^6, Time=ts.string))
	  att <- FALSE
	  break
      }
    }
    if (att) { 
      print(ts.string)
      print(c(0, as.numeric(order.num), Inf))
      latency.data <- rbind(latency.data, data.frame(OrderType=0, OrderRef=as.numeric(order.num), RspTime=Inf, Time=ts.string))
    }
  } else if (fields[1] == "CancelLimitOrderMessage") {
    att <- TRUE
    order.num <- strsplit(fields[11], "=")[[1]][2]
    ts.string <- strsplit(fields[7], "=")[[1]][2]
    time.in <- as.numeric(strsplit(fields[8], "=")[[1]][2])
    for (j in (i+1):length(lines)) {
      fields <- strsplit(lines[j],",")[[1]]
      if (is.element(fields[1], c("CanceledLimitOrderMessage", "RejectCancelLimitOrderMessage")) && 
          strsplit(fields[11], "=")[[1]][2] == order.num) { 
        time.out <- as.numeric(strsplit(fields[8], "=")[[1]][2])
	print(ts.string)
	print(c(1, as.numeric(order.num), round((time.out-time.in)/10^6)))
	latency.data <- rbind(latency.data, data.frame(OrderType=1, OrderRef=as.numeric(order.num), RspTime=(time.out-time.in)/10^6, Time=ts.string))
	att <- FALSE
	break
      }
    }
    if (att) { 
      print(ts.string)
      print(c(1, as.numeric(order.num), Inf))
      latency.data <- rbind(latency.data, data.frame(OrderType=1, OrderRef=as.numeric(order.num), RspTime=Inf, Time=ts.string))
    }
  }
}

latency.data <- latency.data[with(latency.data, order(OrderRef, OrderType)),]

for (i in 1:dim(latency.data)[1]) { 
  row <- latency.data[i,]
  if (is.infinite(row$RspTime)) { 
    print(c("Warning", row$OrderType, row$OrderRef, row$RspTime, row$Time))
  }
  writeLines(paste(row$OrderType, row$OrderRef, row$RspTime, row$Time, sep=","), con=fd, sep="\n")
}

latency.ack <- latency.data[latency.data$OrderType == 0, "RspTime"]
latency.ack <- latency.ack[latency.ack<threshold]

latency.cancel <- latency.data[latency.data$OrderType == 1, "RspTime"]
latency.cancel <- latency.cancel[latency.cancel<threshold]

pdf(pdf.file)
hist(latency.ack, breaks=20)
hist(latency.cancel, breaks=20)
dev.off()