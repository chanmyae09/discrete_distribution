# for i in 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 
#   do 
#   for t in 1 2 4 6 12
#   do
#     ./testscriptChanging_thread.sh 20 $i $t
#   done
# done


for i in 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 
  do 
  ./testscriptChanging_thread_new.sh 20 $i
done
