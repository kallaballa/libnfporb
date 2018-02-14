truncate --size 0 testgenfuzz.log

while [ 0 ]; do
  echo -n "#### Running case $line: "
  examples/fuzz "`ls data/generated | shuf | head -n1`" "`ls data/generated | shuf | head -n1`" &>> testgenfuzz.log
  if [ $? -eq 0 ]; then
    echo Success
  else
    echo Fail
    exit 1;
  fi
done
