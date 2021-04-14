from flask import Flask,render_template,request,redirect,url_for,session
import config
from models import User,Weibo,Answer
from exts import db
from decorators import login_required
from datetime import datetime

#Flask类只有一个必须指定的参数，即程序主模块或者包的名字，__name__是系统变量，该变量指的是本py文件的文件名"""
app=Flask(__name__)
#加载配置文件
app.config.from_object(config)
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = True
#让配置生效
db.init_app(app)

#首页函数
@app.route('/')
def index():
    context={
        'weibos':Weibo.query.all()
    }
    return render_template('index.html',**context)

if __name__=='__main__':
    app.run()