args <- commandArgs(T)

data.folder <- "/home/fye/tmp"
filenames <- dir(data.folder)
idx <- grep(".........SpreadMM.IF.....IF.....log", filenames)
filenames <- filenames[idx]

miss.front <- c(0, 0)    # number of missing, total cost of hedging
miss.back <- c(0, 0)
pnl <- c(0, 0)           # number of rounds, total pnl of trading
fill <- 0

for (filename in filenames) { 
  print(filename)
  splits.filename <- strsplit(filename, "\\.")[[1]]
  symbol.f <- splits.filename[3]
  symbol.b <- splits.filename[4]
  lines <- readLines(filename)
  pos <- c(0, 0)
  exec <- c(0, 0)
  for (line in lines) { 
    fields <- strsplit(line,",")[[1]]
    if (fields[1] == "FilledOrderMessage") { 
       px <- as.numeric(strsplit(fields[2], "=")[[1]][2])
       side <- as.numeric(strsplit(fields[3], "=")[[1]][2])
       contract <- strsplit(fields[6], "=")[[1]][2]
       if ((abs(pos[1]) + abs(pos[2])) == 0) { 
         if (contract == symbol.f) { 
	   pos[1] <- side
	   exec[1] <- px
	 } else { 
	   pos[2] <- side
	   exec[2] <- px
	 }
	 almost <- FALSE
	 fill <- fill + 1
       }
       else if ((abs(pos[1]) + abs(pos[2])) == 1) { 
         if (contract == symbol.f && pos[1] != 0) { 
	   pos[1] <- pos[1] + side
	   if (pos[1] == 0 && !almost) { 
	     miss.back[1] <- miss.back[1] + 1
	     miss.back[2] <- miss.back[2] - (pos[1] * exec[1] + px * side)
	   }
	   else if (pos[1] == 0 && almost) { 
	     pnl[2] <- pnl[2] - (pos[1] * exec[1] + px * side)
	   }
	   else { 
	     fill <- fill + 1
	     exec[1] <- (exec[1] * (pos[1] - side) + px * side) / pos[1]
	   }
	 }
         else if (contract == symbol.b && pos[2] != 0) { 
	   pos[2] <- pos[2] + side
	   if (pos[2] == 0 && !almost) { 
	     miss.front[1] <- miss.front[1] + 1
	     miss.front[2] <- miss.front[2] - (pos[2] * exec[2] + px * side)
	   } 
	   else if (pos[2] == 0 && almost) { 
	     pnl[2] <- pnl[2] - (pos[2] * exec[2] + px * side)
	   }
	   else { 
	     fill <- fill + 1
	     exec[2] <- (exec[2] * (pos[2] - side) + px * side) / pos[2]
	   }
	 }
	 else if (contract == symbol.f && pos[2] != 0) { 
	   pos[1] <- side
	   exec[1] <- px
	   if (pos[1] * pos[2] < 0) { 
	     pnl[1] <- pnl[1] + 1
	   } else { 
	     fill <- fill + 1
	   }
	 }
	 else if (contract == symbol.b && pos[1] != 0) { 
	   pos[2] <- side
	   exec[2] <- px
	   if (pos[1] * pos[2] < 0) { 
	     pnl[1] <- pnl[1] + 1
	   } else { 
	     fill <- fill + 1
	   }
	 }
       }
       else if (abs(pos[1]) == 1 && abs(pos[2]) == 1) { 
         if (contract == symbol.f && pos[1] * side < 0) { 
	   almost <- TRUE
	   pnl[2] <- pnl[2] - (pos[1] * exec[1] + px * side)
	   pos[1] <- 0
	 }
	 else if (contract == symbol.b && pos[2] * side < 0) { 
	   almost <- TRUE
	   pnl[2] <- pnl[2] - (pos[2] * exec[2] + px * side)
	   pos[2] <- 0
	 }
       }  
    }
  }
}

print(miss.front)
print(miss.back)
print(pnl)
print(fill)
