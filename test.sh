truncate --size 0 test.log

if [ ! -x examples/nfp ]; then
  echo "You need to build the examples to run the tests"
  exit 2
fi

ls -d data/handcrafted/* | while read line; do
  echo -n "#### Running case $line: "
  examples/nfp $line/A.wkt $line/B.wkt &>> test.log
  if [ $? -eq 0 ]; then
    echo Success
#    mv nfp.svg `basename $line`-nfp.svg  
  else
    echo Fail
    exit 1
  fi
done
