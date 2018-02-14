truncate --size 0 testgen.log

while [ 0 ]; do
  echo -n "#### Running case $line: "
  examples/nfp data/generated/"`ls data/generated | shuf | head -n1`" data/generated/"`ls data/generated | shuf | head -n1`" &>> testgen.log
  if [ $? -eq 0 ]; then
    echo Success
  else
    echo Fail
    exit 1;
  fi
done
