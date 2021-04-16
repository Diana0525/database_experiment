from flask import Flask,render_template,request,redirect,url_for,session
# import config
from models import User,Weibo,Answer
import mysql.connector
from exts import db
from decorators import login_required
from datetime import datetime

conn=mysql.connector.connect(user="root",password="mysql",database="testdb")#数据库连接
cursor=conn.cursor()

#Flask类只有一个必须指定的参数，即程序主模块或者包的名字，__name__是系统变量，该变量指的是本py文件的文件名"""
app=Flask(__name__)

# #加载配置文件
# app.config.from_object(config)
# app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = True
# #让配置生效
# db.init_app(app)

#首页函数
@app.route('/',methods=['GET'])
def index():
    # context={
    #     'weibos':Weibo.query.all()
    # }
    # return render_template('index.html',**context)
    return render_template('index.html')


# 登录函数
# @app.route('/login',methods=['GET','POST'])
# def login():
#     if request.method=='GET':
#         return render_template('login.html')
if __name__ == '__main__':
    app.run()