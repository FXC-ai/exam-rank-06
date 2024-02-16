osascript -e 'tell app "Terminal"
    do script "cd ~/Desktop/exam-rank06 && gcc -Wall -Wextra -Werror mini_serv.c -o mini_serv &&./mini_serv"
end tell'

sleep 2

osascript -e 'tell app "Terminal"
    do script "cd ~/Desktop/exam-rank06 && gcc -Wall -Wextra -Werror client_reader.c -o client_r &&./client_r"
end tell'

sleep 2

osascript -e 'tell app "Terminal"
    do script "cd ~/Desktop/exam-rank06 && gcc -Wall -Wextra -Werror client_reader.c -o client_r &&./client_r"
end tell'

sleep 2

osascript -e 'tell app "Terminal"
    do script "cd ~/Desktop/exam-rank06 && nc localhost 8081"
end tell'