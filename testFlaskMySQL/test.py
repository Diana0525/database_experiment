from flask import Flask,render_template,redirect,session,url_for,escape
from flask import request
from datetime import datetime
import os
import mysql.connector
conn=mysql.connector.connect(user="root",password="mysql",database="weibo")#数据库连接
cursor = conn.cursor()
app=Flask(__name__)#引入Flask
app.config['SECRET_KEY'] = os.urandom(24)
# app.config['PERMANENT_SESSION_LIFETIME'] = timedelta(days=7)

# 查询数据库
def read_sql(conn,sentence):
    cursor = conn.cursor()
    cursor.execute(sentence) #sentence为sql指令
    result = cursor.fetchall()
    conn.commit()
    cursor.close()
    return result  #返回为远元组

@app.route('/',methods=['GET'])#跳转至login.html，请求方式GET
def show():
    users = read_sql(conn,'select user_name,user_psw from userinfo')
    return render_template('index.html',users=users)

@app.route('/index',methods=['GET'])#跳转至login.html，请求方式GET
def index():
    users = read_sql(conn,'select user_name,user_psw from userinfo')
    return render_template('index.html',users=users)

@app.route('/login',methods=['POST', 'GET'])
def login():
    if request.method == 'GET':
        return render_template('login.html')
    else:
        username = request.form['username'] #界面传值
        password = request.form['password'] #界面传值
        if(len(username) == 0 | len(password) == 0):
            return render_template('login.html',text='用户名或密码不能为空')

        # cursor.execute('select username from userinfo')#查询test表查询用户名
        usernames=read_sql(conn,'select user_name,user_ID from userinfo')

        for user in usernames:
            if request.form['username']==user[0]:
                cursor.execute('select user_psw '
                               'from userinfo '
                               'where user_name=%s',(user[0],))
                pw=cursor.fetchall()#从test表中获取密码
                if request.form['password']==pw[0][0]:#如果页面输入的password匹配test表返回的密码
                    session['username'] = username  # 将目前登录的用户名加入session
                    return render_template('index.html',user=user[0])
                return '<h>账号、密码错误！</h>'

        # cursor.close()#关闭游标
        # conn.close()#关闭连接

@app.route('/regist',methods=['POST', 'GET'])#表单提交
def regist():
    if request.method == 'GET':
        return render_template('regist.html')
    else:
        user = request.form.get('username')
        cursor.execute('select user_name '
                       'from userinfo '
                       'where user_name = %s',(user,))
        back = cursor.fetchall()
        if any(back):
            return render_template('regist.html', text='用户名已存在，请重新注册')
        pw1 = request.form.get('password1')
        pw2 = request.form.get('password2')
        if pw1 == pw2:
            cursor.execute("insert into userinfo(user_name, user_psw, user_state) "
                           "values ('%s', '%s', '%s')"
                           % (user, pw1, '1'))     # 把注册信息加入test表
            conn.commit()
            return '<h>注册成功！请登录。</h><form action="/login" method="get"><p><button type="submit">' \
                   '返回登录</button></p></form>'
        else:
            return render_template('regist.html',text='两次输入的密码不匹配,请重新输入')

        # cursor.close()#关闭游标
        # conn.close()#关闭连接

@app.route('/release',methods=['POST', 'GET'])# 发布微博
def release():
    if request.method == 'GET':
        username = session.get('username')
        return render_template('release.html',user=username)
    else:
        username = session.get('username')
        print(username)
        cursor.execute('select user_ID '
                                'from userinfo '
                                'where user_name = %s',(username,))
        user_id = cursor.fetchall()
        print(user_id[0][0])
        print('post')
        article = request.form.get('article')
        print(article)
        message_time =  str(datetime.now().strftime("%Y-%m-%d %H:%M:%S")) #当前时间
        print(message_time)
        cursor.execute("insert into message1(user_ID, message_time, valid) "
                       "values (%d, '%s', '%s')"
                       % (user_id[0][0],message_time,'1'))
        conn.commit()
        return '<h>发表成功！</h><form action="/index" method="get"><p><button type="submit">返回主页</button></p></form>'

# 注销函数
@app.route('/logout/')
def logout():
    return redirect(url_for('login'))

if __name__=='__main__':
    app.run()
