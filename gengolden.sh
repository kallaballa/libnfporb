truncate --size 0 test.log

ls -d data/handcrafted/* data/generated/* | while read line; do
  echo -n "#### Running case $line: "
  examples/writeGolden $line/A.wkt $line/B.wkt $line/golden.wkt &>> test.log
  if [ $? -eq 0 ]; then
    echo Success
  else
    echo Fail
  fi
done
