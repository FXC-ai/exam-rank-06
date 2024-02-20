osascript -e 'tell app "Terminal"
    do script "cd ~/Desktop/exam-rank06 && gcc -Wall -Wextra -Werror mini_serv.c -o mini_serv && leaks -atExit -- ./mini_serv 8001"
end tell'

sleep 2

osascript -e 'tell app "Terminal"
    do script "cd ~/Desktop/exam-rank06 && nc localhost 8001"
end tell'

sleep 2

osascript -e 'tell app "Terminal"
    do script "cd ~/Desktop/exam-rank06 && nc localhost 8001"
end tell'

sleep 2

osascript -e 'tell app "Terminal"
    do script "cd ~/Desktop/exam-rank06 && nc localhost 8001"
end tell'