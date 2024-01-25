1.最初使用Kleopatra生成了密钥对；
2.但Kleopatra在解码非 .gpg 文件时出现报错，因此改用命令行；
3.导入TAJJ的public key：
gpg --import [ta-public-key.txt]
4.查看已有密钥：
gpg --list-keys
5.decrypt,输出文件名为 学号_afterd.jpeg:
gpg 学号_d.jpeg
6.encrypt + sign:
gpg --armor --sign --encrypt --recipient ta_mac@163.com --local-user 学号@zju.edu.cn 学号_afterd.jpeg