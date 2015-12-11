./start 5 &
sleep 1
./producer 1 5 &
./consumer 1 &
