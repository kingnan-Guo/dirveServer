# github
ssh -T git@github.com

git config --global user.name "你的用户名"
git config --global user.email "你的邮箱"


ls -al ~/.ssh
ssh-keygen -t ed25519 -C "你的邮箱"


# 查看公钥内容，复制输出内容
cat ~/.ssh/id_ed25519.pub



git fetch --all