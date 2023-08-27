# CPP-WTFSC
Write The Friendly Source Code In CPP！

# RUN
```
cd CPP-WTFSC

docker build -t my_env .

sudo docker run -it -p 127.0.0.1:8080:8080 -v ${PWD}:/home/zhuziyi/CPP-WTFSC -w /home/zhuziyi/CPP-WTFSC my_env 
```