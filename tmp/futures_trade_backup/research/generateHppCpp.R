convert.name <- function(string)
{
  if(string == "bookPressure") {
    return ("BookPressure")
  }
  if(length(grep("^FeatureProd", string)) > 0) {
    aa <- strsplit(string, "-")
    component <- gsub("^FeatureProd\\.", "", string)
    if(length(grep("\\.\\.", component)) > 0) {
      comp1 <- convert.name(paste(strsplit(component, "\\.\\.")[[1]][1], ".", sep=""))
      comp2 <- convert.name(strsplit(component, "\\.\\.")[[1]][2])
    } else {
      comp1 <- convert.name(strsplit(component, "\\.")[[1]][1])
      comp2 <- convert.name(gsub("^\\.", "", gsub(strsplit(component, "\\.")[[1]][1], "", component)))
    }
    return(sprintf("FeatureProd<%s, %s >", comp1, comp2))
  }
  if(length(grep("^FeatureAgainst\\.", string)) > 0) {
    aa <- strsplit(string, "-")
    component <- gsub("^FeatureAgainst\\.", "", string)
    if(length(grep("\\.\\.", component)) > 0) {
      comp1 <- convert.name(paste(strsplit(component, "\\.\\.")[[1]][1], ".", sep=""))
      comp2 <- convert.name(strsplit(component, "\\.\\.")[[1]][2])
    } else {
      comp1 <- convert.name(strsplit(component, "\\.")[[1]][1])
      comp2 <- convert.name(gsub("^\\.", "", gsub(strsplit(component, "\\.")[[1]][1], "", component)))
    }
    return(sprintf("FeatureAgainst<%s, %s >", comp1, comp2))
  }
  if(length(grep("^FeatureAgainstLag\\.", string)) > 0) {
    aa <- strsplit(string, "-")
    component <- gsub("^FeatureAgainstLag\\.", "", string)
    if(length(grep("\\.\\.", component)) > 0) {
      comp1 <- convert.name(paste(strsplit(component, "\\.\\.")[[1]][1], ".", sep=""))
      comp2 <- convert.name(strsplit(component, "\\.\\.")[[1]][2])
    } else {
      comp1 <- convert.name(strsplit(component, "\\.")[[1]][1])
      comp2 <- convert.name(gsub("^\\.", "", gsub(strsplit(component, "\\.")[[1]][1], "", component)))
    }
    return(sprintf("FeatureAgainstLag<%s, %s >", comp1, comp2))
  }
  if(length(grep("^FeatureAgree\\.", string)) > 0) {
    aa <- strsplit(string, "-")
    component <- gsub("^FeatureAgree\\.", "", string)
    if(length(grep("\\.\\.", component)) > 0) {
      comp1 <- convert.name(paste(strsplit(component, "\\.\\.")[[1]][1], ".", sep=""))
      comp2 <- convert.name(strsplit(component, "\\.\\.")[[1]][2])
    } else {
      comp1 <- convert.name(strsplit(component, "\\.")[[1]][1])
      comp2 <- convert.name(gsub("^\\.", "", gsub(strsplit(component, "\\.")[[1]][1], "", component)))
    }
    return(sprintf("FeatureAgree<%s, %s >", comp1, comp2))
  }
  if(length(grep("^FeatureAgreeLag\\.", string)) > 0) {
    aa <- strsplit(string, "-")
    component <- gsub("^FeatureAgreeLag\\.", "", string)
    if(length(grep("\\.\\.", component)) > 0) {
      comp1 <- convert.name(paste(strsplit(component, "\\.\\.")[[1]][1], ".", sep=""))
      comp2 <- convert.name(strsplit(component, "\\.\\.")[[1]][2])
    } else {
      comp1 <- convert.name(strsplit(component, "\\.")[[1]][1])
      comp2 <- convert.name(gsub("^\\.", "", gsub(strsplit(component, "\\.")[[1]][1], "", component)))
    }
    return(sprintf("FeatureAgreeLag<%s, %s >", comp1, comp2))
  }
  if(length(grep("^TradeBreakOut\\.", string)) > 0) {
    aa <- strsplit(string, "\\.")
    return(sprintf("TradeBreakOut(%s,%s,%s)", aa[[1]][2], aa[[1]][4], aa[[1]][6]))
  }
  if(length(grep("^EMAWindowedBreakthrough\\.", string)) > 0) {
    aa <- strsplit(string, "\\.")
    return(sprintf("EMAWindowedBreakthrough<%s,%s>", aa[[1]][2], aa[[1]][4]))
  }
  if(length(grep("^WindowedBreakThrough\\.", string)) > 0) {
    aa <- strsplit(string, "\\.")
    return(sprintf("WindowedBreakthrough<%s>", aa[[1]][2]))
  }
  if(length(grep("^BookPressureNL\\.", string)) > 0) {
    aa <- strsplit(string, "\\.")
    return(sprintf("BookPressureNL<%s, %s>", aa[[1]][2], aa[[1]][3]))
  }
  if(string == "BookPressureLog") {
    return ("BookPressureLog")
  }
  if(length(grep("^ShortTermReturn", string)) > 0) {
    aa <- gsub("\\.Millisec\\.", "",  gsub("ShortTermReturn\\.", "", string))
    return(sprintf("ShortTermReturn<%s>", aa))
  }
  if(string == "TradeDeviation") {
    return(string)
  }
  if(length(grep("^TradeDeviationMA", string)) > 0) {
    aa <- gsub("\\.", "", gsub("TradeDeviationMA\\.", "", string))
    return(sprintf("TradeDeviationMA<%s>", aa))
  }
  if(length(grep("^SignedVolume\\.", string)) > 0) {
    aa <- gsub("\\.Millisec\\.", "", gsub("SignedVolume\\.", "", string))
    return(sprintf("SignedVolume<%s>", aa))
  }
  if(length(grep("^SignedVolumeTradeVsMid\\.", string)) > 0) {
    aa <- gsub("\\.Millisec\\.", "", gsub("SignedVolumeTradeVsMid\\.", "", string))
    return(sprintf("SignedVolumeTradeVsMid<%s>", aa))
  }
  if(length(grep("^SignedVolumeWithBookSizeChg\\.", string)) > 0) {
    aa <- gsub("\\.Millisec\\.", "", gsub("SignedVolumeWithBookSizeChg\\.", "", string))
    return(sprintf("SignedVolumeWithBookSizeChg<%s>", aa))
  }
  if(length(grep("^VwapMidReturn\\.", string)) > 0) {
    aa <- gsub("\\.Millisec\\.", "", gsub("VwapMidReturn\\.", "", string))
    return(sprintf("VwapMidReturn<%s>", aa))
  }
  if(length(grep("^BookSizeChg\\.", string)) > 0) {
    aa <- gsub("\\.Millisec\\.Ask\\.", "", gsub("\\.Millisec\\.Bid\\.", "", gsub("BookSizeChg\\.", "", string)))
    bb <- ifelse(length(grep("Bid\\.", string)) > 0, "static_cast<int>(BuySellSide::buy)",
                 "static_cast<int>(BuySellSide::sell)")
    return(sprintf("BookSizeChg<%s, %s>", aa, bb))
  }
  if(length(grep("^BookSizeChg2\\.", string)) > 0) {
    aa <- gsub("\\.Millisec\\.Ask\\.", "", gsub("\\.Millisec\\.Bid\\.", "", gsub("BookSizeChg2\\.", "", string)))
    bb <- ifelse(length(grep("Bid\\.", string)) > 0, "static_cast<int>(BuySellSide::buy)",
                 "static_cast<int>(BuySellSide::sell)")
    return(sprintf("BookSizeChg2<%s, %s>", aa, bb))
  }
  return(NULL)
}

generate.signal <- function(nms)
{
  string <- NULL
  for(nm in nms) {
    cnm <- convert.name(nm)
    if(length(grep("^TradeBreakOut", cnm)) > 0) {
      string <- c(string, "TradeBreakOut")
    } else {
      string <- c(string, cnm)
    }
  }
  return(paste(string, collapse=",\n"))
}

generate.register <- function(nms)
{
  string <- NULL
  for(nm in nms) {
    cnm <- convert.name(nm)
    if(length(grep("^TradeBreakOut", cnm)) > 0) {
      string <- c(string, sprintf("PFeature(new %s)", cnm))
    } else {
      string <- c(string, sprintf("PFeature(new %s())", cnm))
    }
  }
  return(paste(string, collapse=",\n"))
}

generate.beta <- function(coefs)
{
  return(paste(coefs, collapse=", "))
}

generate.bound <- function(sds)
{
  string <- NULL
  for(ii in 1:length(sds)) {
    string <- c(string,
                sprintf("directionSignal_.getFeatures()[%d]->setBound(true, %f);",
                        ii - 1,
                        sds[ii]))
  }
  return(paste(string, collapse="\n"))
}


write.cpp <- function(ticker.head, template.cpp, folder)
{
  template <- paste(readLines(file(template.cpp, "r")), collapse="\n")
  ticker.head <- toupper(ticker.head)
  filename <- paste(folder, sprintf("%s.cpp", ticker.head), sep="")
  text <- sprintf(template, ticker.head, ticker.head)
  cat(text, file=filename)
}
                      

write.hpp <- function(ticker.head, template.hpp, folder,
                      signal, signal.register, beta, bounds)
{
  template <- paste(readLines(file(template.hpp, "r")), collapse="\n")
  ticker.head <- toupper(ticker.head)
  filename <- paste(folder, sprintf("%s.hpp", ticker.head), sep="")
  text <- sprintf(template,
                  ticker.head, ticker.head,
                  ticker.head,
                  signal,
                  ticker.head,
                  signal.register,
                  beta,
                  bounds,
                  ticker.head,
                  ticker.head)
  cat(text, file=filename)
}
