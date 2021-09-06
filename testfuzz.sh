truncate --size 0 testfuzz.log
if [ ! -x examples/nfp ]; then
  echo "You need to build the examples to run the tests"
  exit 2
fi

ls -d data/handcrafted/* | while read line; do
  echo -n "#### Running case $line: "
  examples/fuzz $line/A.wkt $line/B.wkt &>> testfuzz.log
  if [ $? -eq 0 ]; then
    echo Success
  else
    echo Fail
    exit 1;
  fi
done
