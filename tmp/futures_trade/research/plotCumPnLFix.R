args <- commandArgs(T)
pdf.file <- args[[1]]

pnl.after.commission <- -c(-1530.0, -7500.0, -2220.0, -4200.0, -1740, -3540.0, -4950.0, -5640.0, -5730.0, 150.0, -570.0, -180.0, -1290.0, -210.0, -1260.0, -780.0, -1380.0, -240.0, -28300.0, -3840.0, -1680.0, -240.0, -900.0, -2520.0, -1920.0, -60.0, -4260.0, -600.0, -960.0, 240.0, -600.0, 300.0, -120.0, -1980.0, -4200.0, 120.0, -480.0, -420.0, -390.0, -600.0, -900.0, -30.0, 240.0, -180.0, -60.0, -840.0, -120.0, 120.0, -4560.0, 0, -1020.0, -900.0, -240.0, 0.0, 1050.0, -240.0, 1080.0, -240.0, -60.0, -3360.0, 480.0, -240.0)
cum.pnl.after.commission <- cumsum(pnl.after.commission)

initial <- 800000
equity.curve <- cum.pnl.after.commission + initial

num.days <- length(equity.curve)
first <- initial
last <- equity.curve[num.days]
return.period <- round((last-first) / first * 100, 2)
return.annual <- round(return.period / num.days * 252, 2)

return.after.commission <- round(pnl.after.commission / initial * 100, 2)
return.min <- min(return.after.commission)
return.max <- max(return.after.commission)

pdf(pdf.file)
plot(equity.curve, type="l", col="blue", xlab="Date", ylab="Equity Curve", main=paste("Period Return (%):", return.period, "Annual Return (%):", return.annual))
barplot(return.after.commission, xlab="Date", ylab="Daily Return (%)", main=paste("Max Daily Return:",return.max,"Min Daily Return",return.min))
dev.off()