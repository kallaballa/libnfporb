truncate --size 0 testgenfuzz.log

while [ 0 ]; do
  file1="`ls data/generated/* | shuf | head -n1`"
  file2="`ls data/generated/* | shuf | head -n1`" 

  echo -n "#### Running case $file1 $file2: "
  examples/fuzz $file1 $file2 &>> testgenfuzz.log
  if [ $? -eq 0 ]; then
    echo Success
  else
    echo Fail
    exit 1;
  fi
done
