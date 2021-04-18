from flask import Flask, render_template, redirect, session, url_for, escape
from flask import request
from datetime import datetime
import os
import mysql.connector

conn = mysql.connector.connect(user="root", password="mysql", database="weibo")  # 数据库连接
cursor = conn.cursor()
app = Flask(__name__)  # 引入Flask
app.config['SECRET_KEY'] = os.urandom(24)


# app.config['PERMANENT_SESSION_LIFETIME'] = timedelta(days=7)

# 查询数据库
def read_sql(conn, sentence):
    cursor = conn.cursor()
    cursor.execute(sentence)  # sentence为sql指令
    result = cursor.fetchall()
    conn.commit()
    cursor.close()
    return result  # 返回为远元组


@app.route('/', methods=['GET'])  # 跳转至login.html，请求方式GET
def show():
    message = [['亲爱的用户:',
               str(datetime.now().strftime("%Y-%m-%d %H:%M:%S")),
               '欢迎来到旧浪微博']]
    return render_template('index.html', messages=message)


@app.route('/index', methods=['GET'])  # 跳转至index.html，请求方式GET
def index():
    article_message = sel_user_article()
    username = session.get('username')
    return render_template('index.html', user=username, messages=article_message)

@app.route('/login', methods=['POST', 'GET'])
def login():
    if request.method == 'GET':
        return render_template('login.html')
    else:
        username = request.form['username']  # 界面传值
        password = request.form['password']  # 界面传值
        if (len(username) == 0 | len(password) == 0):
            return render_template('login.html', text='用户名或密码不能为空')

        # cursor.execute('select username from userinfo')#查询test表查询用户名
        usernames = read_sql(conn, 'select user_name,user_ID from userinfo')

        for user in usernames:
            if request.form['username'] == user[0]:
                cursor.execute('select user_psw '
                               'from userinfo '
                               'where user_name=%s', (user[0],))
                pw = cursor.fetchall()  # 从test表中获取密码
                if request.form['password'] == pw[0][0]:  # 如果页面输入的password匹配test表返回的密码
                    session['username'] = username  # 将目前登录的用户名加入session
                    article_message = sel_user_article()
                    return render_template('index.html', user=username, messages=article_message)
                return render_template('login.html', text='账号、密码错误！')

        # cursor.close()#关闭游标
        # conn.close()#关闭连接


@app.route('/regist', methods=['POST', 'GET'])  # 表单提交
def regist():
    if request.method == 'GET':
        return render_template('regist.html')
    else:
        user = request.form.get('username')
        cursor.execute('select user_name '
                       'from userinfo '
                       'where user_name = %s', (user,))
        back = cursor.fetchall()
        if any(back):
            return render_template('regist.html', text='用户名已存在，请重新注册')
        pw1 = request.form.get('password1')
        pw2 = request.form.get('password2')
        if pw1 == pw2:
            cursor.execute("insert into userinfo(user_name, user_psw, user_state) "
                           "values ('%s', '%s', '%s')"
                           % (user, pw1, '1'))  # 把注册信息加入test表
            conn.commit()
            return '<h>注册成功！请登录。</h><form action="/login" method="get"><p><button type="submit">' \
                   '返回登录</button></p></form>'
        else:
            return render_template('regist.html', text='两次输入的密码不匹配,请重新输入')

        # cursor.close()#关闭游标
        # conn.close()#关闭连接


@app.route('/release', methods=['POST', 'GET'])  # 发布微博
def release():
    if request.method == 'GET':
        username = session.get('username')
        section_name = read_sql(conn,'select section_name from section')
        return render_template('release.html', user=username,sections=section_name)
    else:
        user_id = get_user_id()
        article = request.form.get('article')
        message_time = str(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))  # 当前时间
        cursor.execute("insert into message1(user_ID, message_time, valid) "
                       "values (%d, '%s', '%s')"
                       % (user_id[0][0], message_time, '1'))
        conn.commit()
        t_article_ID = 0  # 若是原创文章，此项默认为0
        article_type = 0  # 原创为0，转发为1
        section_name = request.form.get('section_name')
        cursor.execute('select section_ID from section where section_name = %s ',(section_name,))
        section_ID = cursor.fetchall()
        message_ID = read_sql(conn,'select @@identity') #找到最新插入表的信息的Message_id
        cursor.execute("insert into article(message_ID,section_ID,t_article_ID,article,article_type) "
                       "values (%d, %d, %d, '%s', %d) "
                       % (message_ID[0][0], section_ID[0][0], t_article_ID, article, article_type))
        conn.commit()
        return '<h>发表成功！</h><form action="/index" method="get"><p><button type="submit">返回主页</button></p></form>'

# 注销函数
@app.route('/logout/')
def logout():
    session.pop('username')
    return redirect(url_for('login'))

# 编辑用户详细信息
@app.route('/user_detail_edit',methods=['POST', 'GET'])
def user_detail_edit():
    if request.method=='GET':
        return render_template('user_detailinfo.html', user = session.get('username'))
    else:
        sex = request.form.get('sex')
        print(sex)
        education = request.form.get('education')
        print(education)
        job = request.form.get('job')
        print(job)
        address = request.form.get('address')
        print(address)
        individual_resume = request.form.get('individual_resume')
        print(individual_resume)
        phone = request.form.get('phone')
        print(phone)
        mailbox = request.form.get('mailbox')
        print(mailbox)
        user_ID = get_user_id()
        print(user_ID[0][0])
        cursor.execute("insert into user_detail(user_ID, sex, education, job, address, individual_resume,"
                       "phone, mailbox ) "
                       "values (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s') "
                       % (user_ID[0][0], sex, education, job, address, individual_resume, phone
                          ,mailbox, ))
        print(111)
        conn.commit()
        return '<h>保存成功！</h><form action="/user_detail" method="get"><p><button type="submit">' \
               '返回个人详细信息页面</button></p></form>'
#展示用户详细信息
@app.route('/user_detail',methods=[ 'GET'])
def user_detail():
    user_ID = get_user_id()
    cursor.execute("select sex,education,job,address,individual_resume,phone,mailbox"
                   "from user_detail"
                   "where user_ID = '%s'", ( user_ID, ))
    messages = cursor.fetchall()
    return render_template('user_detail_show.html',
                           user = session.get('username'),
                           username = session.get('username'),
                           messagess = messages)
# 向section表插入板块名称
def insert_section(message):
    cursor.execute("insert into section(section_name) "
                   "values ('%s') "
                   "ON DUPLICATE KEY UPDATE section_name='%s' "
                   % (message, message, ))
    conn.commit()
    return

 # 初始化文章模块类型
def section_ini():
    insert_section('日常生活')
    insert_section('社交')
    insert_section('娱乐')
    insert_section('新闻')
    insert_section('体育')
    insert_section('学习')
    return

# 获取当前用户发的文章
def sel_user_article():
    username = session.get('username')
    cursor.execute('select user_ID '
                   'from userinfo '
                   'where user_name = %s', (username,))
    user_id = cursor.fetchall()
    cursor.execute('select userinfo.user_name,article.article,message1.message_time '
                   'from article,message1,userinfo '
                   'where article.message_ID = message1.message_ID '
                   'and message1.user_ID = userinfo.user_ID '
                   'and userinfo.user_ID = %s ', (user_id[0][0],))
    article_message = cursor.fetchall()
    return article_message

# 获取当前用户ID
def get_user_id():
    username = session.get('username')
    cursor.execute('select user_ID '
                   'from userinfo '
                   'where user_name = %s', (username,))
    user_id = cursor.fetchall()
    return user_id
if __name__ == '__main__':
    section_ini()
    app.run()
