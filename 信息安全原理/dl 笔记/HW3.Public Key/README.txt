1.���ʹ��Kleopatra��������Կ�ԣ�
2.��Kleopatra�ڽ���� .gpg �ļ�ʱ���ֱ�����˸��������У�
3.����TAJJ��public key��
gpg --import [ta-public-key.txt]
4.�鿴������Կ��
gpg --list-keys
5.decrypt,����ļ���Ϊ ѧ��_afterd.jpeg:
gpg ѧ��_d.jpeg
6.encrypt + sign:
gpg --armor --sign --encrypt --recipient ta_mac@163.com --local-user ѧ��@zju.edu.cn ѧ��_afterd.jpeg