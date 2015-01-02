library("foba")
setwd('../features/')

train_size <- 2 * 60 * 60 * 4.5
test_size <- 2 * 60 * 60

data <- read.csv('features.IF1302.csv', header = TRUE)

X <- data[, !(names(data) %in% c("ts", "datetime", "MidPoint", "MidPoint_2000ms", 
     	    		         "MidPoint_5000ms", "MidPoint_10000ms", 
				 "MidPoint_20000ms", "MidPoint_30000ms", 
				 "MidPoint_40000ms", "MidPoint_50000ms", 
				 "MidPoint_60000ms"))]
Y <- data[, names(data) %in% c("MidPoint", "MidPoint_2000ms", "MidPoint_5000ms", 
     	    		       "MidPoint_10000ms", "MidPoint_20000ms", 
			       "MidPoint_30000ms", "MidPoint_40000ms", 
			       "MidPoint_50000ms", "MidPoint_60000ms")]
Y <- (Y[,2] - Y[,1]) / Y[,1]

num_sample <- dim(X)[1]
num_features <- dim(X)[2]

feature_count <- matrix(0, nrow = num_features, ncol = num_features)

train_start_index <- 1
train_end_index <- train_start_index + train_size - 1
test_start_index <- train_end_index + 1
test_end_index <- test_start_index + test_size - 1

while (test_end_index <= num_sample){
      model <- foba(X[train_start_index:train_end_index,], Y[train_start_index:train_end_index], intercept=FALSE)
      for (i in 1:num_features){
      	  py <- predict(model, X[test_start_index:test_end_index,], k=i, type="fit")
	  fit <- py$fit
	  px <- predict(model, k=i, type="coefficients")
	  vari <- px$selected.variables

	  feature_count[i,vari] <- feature_count[i,vari] + rep(1,length(vari))
	  }

	  train_start_index <- train_end_index + test_size
	  train_end_index <- train_start_index + train_size - 1
	  test_start_index <- train_end_index + 1
	  test_end_index <- test_start_index + test_size - 1
}

pdf("features.IF1302.pdf")
for (i in 1:num_features){
plot(1:num_features, feature_count[i,], xlab="", ylab="count", main=paste(i, "feature(s)"), ann = FALSE)
axis(1, at = 1:num_features, labels = names(X), las=2)
}
dev.off()