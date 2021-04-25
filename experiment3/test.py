from flask import Flask, render_template, redirect, session, url_for, escape
from flask import request
from datetime import datetime
import os
import mysql.connector
from decorators import login_required

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
    article_message = sel_all_article()
    return render_template('index.html', messages=article_message)

@app.route('/index', methods=['GET'])  # 跳转至index.html，请求方式GET
@login_required
def index():
    article_message = sel_all_article()
    username = session.get('username')
    section_name = read_sql(conn, 'select section_name from section')
    return render_template('index.html', user=username, messages=article_message, types=section_name)

# 针对文章的Message_ID，寻找相关的评论
def find_comment(message_ID):
    cursor.execute("select message1.message_ID,userinfo.user_name,comments.comment_text,message1.message_time "
                   "from userinfo,comments,message1 "
                   "where comments.c_message_ID = %s "
                   "and userinfo.user_ID = message1.user_ID "
                   "and message1.message_ID = comments.message_ID ",(message_ID, ))
    comments = cursor.fetchall()
    return comments

# 针对文章的message_ID,寻找点赞数
def message_praise_num(message_ID):
    cursor.execute("select count(*) as count "
                   "from praise "
                   "where message_ID = %s "
                   "group by message_ID having count>0 ",(message_ID, ))
    praise_num = cursor.fetchall()
    return praise_num

@app.route('/weibo_detail/<message_ID>/<flag>',methods = ['POST','GET'])
@login_required
def weibo_detail(message_ID,flag):
    article_message = sel_ID_article(message_ID)
    username = session.get('username')
    section_name = read_sql(conn, 'select section_name from section')
    comment_list = find_comment(message_ID)
    praise_num = message_praise_num(message_ID)
    if praise_num == []:
        praise_num = [(0,)]
    use_user_name = article_message[0][0]  # 文章发布者，被关注人的name
    use_user_ID = sel_id_from_name(use_user_name) #被关注人的ID
    user_ID = get_user_id()  # 关注者ID，即当前用户

    # 判断用户和博主是否是同一个人
    if user_ID == use_user_ID:
        return render_template('weibo_detail.html', user=username, message=article_message[0], types=section_name,
                               comments=comment_list, praise_num=praise_num[0][0])
    # 判断用户是否已经关注博主
    cursor.execute("select group_ID "
                   "from follow "
                   "where user_ID = %s "
                   "and use_user_ID = %s ",(user_ID[0][0], use_user_ID[0][0]))
    group_ID = cursor.fetchall()
    if group_ID != []: #已关注
        return render_template('weibo_detail.html', user=username, message=article_message[0], types=section_name,
                               comments=comment_list, praise_num=praise_num[0][0], follow=1)
    if flag == 'press': #用户未关注博主，且按下了关注按钮
        print(flag)
        cursor.execute("select group_name "
                       "from group_user "
                       "where user_ID = %s ", (user_ID[0][0],))
        groups = cursor.fetchall() # 获取当前用户已经建立好的分组
        return render_template('weibo_detail.html', user=username, message=article_message[0], types=section_name,
                               comments=comment_list, praise_num=praise_num[0][0], not_follow=1,
                               group_choose = 1, groups = groups)
    elif flag == 'choose': #已经选中某项分组
        print(flag)
        group_name = request.form.get('group_name')
        print(group_name)
        # 根据分组名找到分组ID
        cursor.execute("select group_ID "
                       "from group_user "
                       "where group_name = %s ",(group_name,))
        group_ID = cursor.fetchall()
        print(group_ID)
        # 插入到follow关注表中
        print(user_ID)
        print(use_user_ID)
        print(group_ID)
        cursor.execute("insert into follow(user_ID, use_user_ID, group_ID) "
                       "values ('%s', '%s', '%s') "
                       % (user_ID[0][0],use_user_ID[0][0], group_ID[0][0]))
        conn.commit()
        return redirect(url_for('weibo_detail',message_ID = message_ID, flag = 0))

    # 用户没点击关注按钮
    return render_template('weibo_detail.html', user=username, message=article_message[0], types=section_name,
                           comments=comment_list, praise_num=praise_num[0][0], not_follow=1)


@app.route('/home_choose/<home_index>',methods=['POST', 'GET'])
@login_required
def home_choose(home_index):
    section_name = read_sql(conn, 'select section_name from section')
    if home_index == 'all':
        article_message = sel_all_article()
        username = session.get('username')
        return render_template('index.html', user=username, messages=article_message, types=section_name)
    elif home_index == 'follow': #后续改为关注人发的微博
        article_message = sel_user_article()
        username = session.get('username')
        return render_template('index.html', user=username, messages=article_message, types=section_name)
    elif home_index == 'follow_user': # 显示关注的人和关注分组
        article_message = sel_user_article()
        username = session.get('username')
        user_id = get_user_id()
        cursor.execute("select group_name "
                       "from group_user "
                       "where user_ID = %s ",(user_id[0][0],))
        groups = cursor.fetchall() # 获取当前用户建立的所有分组名
        new_group = request.form.get('new_group')
        if new_group != None:
            user_id = get_user_id()
            cursor.execute("insert into group_user(user_ID,group_name) "
                           "values (%d, '%s') "
                           % (user_id[0][0], new_group))
            conn.commit()
        return render_template('index.html', user=username, types=section_name,follow_group =1,
                               groups = groups)
    elif home_index == 'mywb':
        article_message = sel_user_article()
        username = session.get('username')
        return render_template('index.html', user=username, messages=article_message, types=section_name)
    else:
        for type in section_name:
            if home_index == type[0]:
                cursor.execute("select userinfo.user_name,article.article,message1.message_time "
                                "from userinfo, article, section, message1 "
                                "where section.section_name = %s " 
                                "and section.section_ID = article.section_ID "
                                "and article.message_ID = message1.message_ID "
                                "and message1.user_ID = userinfo.user_ID "
                               "order by message1.message_time  desc ",(type[0], ))
                article_message = cursor.fetchall()
                username = session.get('username')
                return render_template('index.html', user=username, messages=article_message, types=section_name)

@app.route('/follow_user/<group>')
@login_required
def follow_user(group):
    print(group)
    user_ID = get_user_id()
    # 获取当前用户在当前分组中关注的人
    cursor.execute("select u2.user_name "
                   "from userinfo u1, "
                   "userinfo u2, "
                   "follow, "
                   "group_user "
                   "where u1.user_ID = %s "
                   "and group_user.group_name = %s "
                   "and u2.user_ID = follow.use_user_ID "
                   "and group_user.group_ID = follow.group_ID ",(user_ID[0][0],group,))
    user_names = cursor.fetchall()
    print(user_names)
    username = session.get('username')
    section_name = read_sql(conn, 'select section_name from section')
    return render_template('follow_user.html',user=username,types=section_name,follow_group=1,group=group,
                           user_names=user_names)

@app.route('/weibo_op/<op_index>/<message_ID>')
@login_required
def weibo_op(op_index, message_ID):
    username = session.get('username')
    if op_index == 'comment':
        return render_template('comment.html',user=username,message_ID = message_ID)
    elif op_index == 'praise':
        user_id = get_user_id()
        praise_time = str(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))  # 当前时间
        cursor.execute("replace into praise(user_ID, message_ID, praise_time)"
                       "values (%d, '%s', '%s') "
                       % (user_id[0][0], message_ID, praise_time))
        conn.commit()
        return redirect(url_for('index'))

@app.route('/comment_release/<message_ID>',methods=['POST','GET'])  # 发布评论
@login_required
def comment_release(message_ID):
    comments = request.form.get('comments')
    user_id = get_user_id()
    message_time = str(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))  # 当前时间
    cursor.execute("insert into message1(user_ID, message_time, valid) "
                   "values (%d, '%s', '%s')"
                   % (user_id[0][0], message_time, '1'))
    conn.commit()
    c_message_ID = message_ID
    message_ID = read_sql(conn, 'select @@identity')  # 找到最新插入表的信息的Message_id
    cursor.execute("insert into comments(message_ID, comment_text,c_message_ID) "
                   "values (%s, '%s', '%s') "
                   % (message_ID[0][0], comments, c_message_ID))
    conn.commit()
    return redirect(url_for('index'))

@app.route('/login', methods=['POST', 'GET'])
def login():
    if request.method == 'GET':
        return render_template('login.html')
    else:
        username = request.form['username']  # 界面传值
        password = request.form['password']  # 界面传值
        if (len(username) == 0 | len(password) == 0):
            return render_template('login.html', text='用户名或密码不能为空')

        usernames = read_sql(conn, 'select user_name,user_ID from userinfo')
        for user in usernames:
            if request.form['username'] == user[0]:
                cursor.execute('select user_psw '
                               'from userinfo '
                               'where user_name=%s', (user[0],))
                pw = cursor.fetchall()  # 从test表中获取密码
                if request.form['password'] == pw[0][0]:  # 如果页面输入的password匹配test表返回的密码
                    session['username'] = username  # 将目前登录的用户名加入session
                    article_message = sel_all_article()
                    section_name = read_sql(conn, 'select section_name from section')
                    return render_template('index.html', user=username, messages=article_message,
                                           types=section_name)
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
            return redirect(url_for('login'))
        else:
            return render_template('regist.html', text='两次输入的密码不匹配,请重新输入')

        # cursor.close()#关闭游标
        # conn.close()#关闭连接

@app.route('/release', methods=['POST', 'GET'])  # 发布微博
@login_required
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
        return redirect(url_for('index'))

# 注销函数
@app.route('/logout/')
def logout():
    session.pop('username')
    return redirect(url_for('login'))

# 编辑用户详细信息
@app.route('/user_detail_edit',methods=['POST', 'GET'])
@login_required
def user_detail_edit():
    if request.method=='GET':
        return render_template('user_detailinfo.html', user = session.get('username'))
    else:
        sex = request.form.get('sex')
        education = request.form.get('education')
        job = request.form.get('job')
        address = request.form.get('address')
        individual_resume = request.form.get('individual_resume')
        phone = request.form.get('phone')
        mailbox = request.form.get('mailbox')
        user_ID = get_user_id()
        cursor.execute("replace into user_detail(user_ID, sex, education, job, address, individual_resume,"
                       "phone, mailbox ) "
                       "values (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s') "
                       % (user_ID[0][0], sex, education, job, address, individual_resume, phone
                          ,mailbox, ))
        conn.commit()
        cursor.execute("select user_name "
                       "from userinfo "
                       "where user_ID = %s ", (user_ID[0][0],))
        user_name = cursor.fetchall()
        return redirect(url_for('user_detail',user_name = user_name[0][0]))

#展示用户详细信息
@app.route('/user_detail/<user_name>',methods=[ 'GET','POST'])
@login_required
def user_detail(user_name):
        cursor.execute("select user_ID "
                       "from userinfo "
                       "where user_name = %s ", (user_name,))
        user_id = cursor.fetchall()
        # user_id = get_user_id()
        cursor.execute("select sex,education,job,address,individual_resume,phone,mailbox "
                       "from user_detail "
                       "where user_ID = %s ", (user_id[0][0],))
        messages = cursor.fetchall()
        return render_template('user_detail_show.html',
                               user=session.get('username'),
                               username=session.get('username'),
                               messages=messages)

# 向section表插入板块名称
def insert_section(message):
    cursor.execute("insert into section(section_name) "
                   "values ('%s') "
                   "ON DUPLICATE KEY UPDATE section_name='%s' "
                   % (message, message))
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
    cursor.execute('select userinfo.user_name,article.article,message1.message_time,message1.message_ID '
                   'from article,message1,userinfo '
                   'where article.message_ID = message1.message_ID '
                   'and message1.user_ID = userinfo.user_ID '
                   'and userinfo.user_ID = %s '
                   'order by message1.message_time  desc ', (user_id[0][0],))
    article_message = cursor.fetchall()
    return article_message

# 获取所有用户发的文章
def sel_all_article():
    cursor.execute('select userinfo.user_name,article.article,message1.message_time,message1.message_ID '
                   'from article,message1,userinfo '
                   'where article.message_ID = message1.message_ID '
                   'and message1.user_ID = userinfo.user_ID '
                   'order by message1.message_time  desc '
                   )
    article_message = cursor.fetchall()
    return article_message

# 获取某message_ID的文章
def sel_ID_article(message_ID):
    cursor.execute('select userinfo.user_name,article.article,message1.message_time,message1.message_ID '
                   'from article,message1,userinfo '
                   'where message1.message_ID = %s '
                   'and article.message_ID = message1.message_ID '
                   'and message1.user_ID = userinfo.user_ID '
                   'order by message1.message_time  desc ',(message_ID, ))
    article_message = cursor.fetchall()
    return article_message

# 用户经过搜索然后得到有关键字出现的文章
@app.route('/search_article',methods=[ 'GET','POST'])
def search_article():
    search = request.form.get('search')
    print(search)
    cursor.execute("select userinfo.user_name,article.article,message1.message_time,message1.message_ID "
                    "from userinfo,article,message1 "
                    "where article.article like %s "
                    "and userinfo.user_ID = message1.user_ID "
                    "and message1.message_ID = article.message_ID "
                   "order by message1.message_time  desc ",('%'+search+'%', ))
    article_message = cursor.fetchall()
    username = session.get('username')
    section_name = read_sql(conn, 'select section_name from section')
    return render_template('index.html', user=username, messages=article_message, types=section_name)

# 获取当前用户ID
def get_user_id():
    username = session.get('username')
    cursor.execute('select user_ID '
                   'from userinfo '
                   'where user_name = %s', (username,))
    user_id = cursor.fetchall()
    return user_id

# 通过用户名获取用户ID
def sel_id_from_name(user_name):
    cursor.execute("select user_ID "
                   "from userinfo "
                   "where user_name = %s ",(user_name,))
    user_id = cursor.fetchall()
    return user_id
if __name__ == '__main__':
    section_ini()
    app.run()
