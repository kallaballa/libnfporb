truncate --size 0 test.log

ls -d data/* | while read line; do
  echo -n "#### Running case $line: "
  examples/nfp $line/A.wkt $line/B.wkt &>> test.log
  if [ $? -eq 0 ]; then
    echo Success
  else
    echo Fail
    exit 1;
  fi
done
