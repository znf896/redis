read -p "commint message": $message
echo "$branch"
git commit -m "$message"
git branch -M main
git remote add origin git@github.com:znf896/redis.git #连接远端的redis仓库
git push -u origin main


