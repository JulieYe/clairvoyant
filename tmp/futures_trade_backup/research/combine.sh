#!/usr/local/bin/bash

ls features.* | head -n1 | xargs head -n1 > features.IF.csv

ls features.201301* | xargs -L1 tail -n+2 >> features.IF.csv

