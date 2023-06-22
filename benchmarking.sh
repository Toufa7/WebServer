#!/bin/bash


curl --location 'http://localhost:8080/post' --header 'Transfer-Encoding: chunked' --header 'Content-Type: text/plain' --data '@/Users/abouchfa/Desktop/a.txt'

curl --location 'http://localhost:8080/post' --header 'Transfer-Encoding: chunked' --header 'Content-Type: image/jpeg'  --data '@/Users/abouchfa/Desktop/img1.jpg'

curl --location 'http://localhost:8080/post' --header 'Transfer-Encoding: chunked' --header 'Content-Type: video/mp4' --data '@/Users/abouchfa/Desktop/video1.mp4'

curl --location 'http://localhost:8080/post' --header 'Transfer-Encoding: chunked' --header 'Content-Type: text/plain' --data '@/Users/abouchfa/Desktop/txt.txt'