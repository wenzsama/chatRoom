# chatRoom
simple online chat room using c in linux

function include:
1.register
2.log in
3.chat personal
4.chat all
5.see user online
6.kick by admin
7.speak/no speak by admin

cs model:
client: using thread to deal with request and receive sperate
server: using single process +select()

database: sqlite (to manage the register info)
datastructre:     message struct   ; clientinf struct ; using linklist to manage online user(add,delete,find)
