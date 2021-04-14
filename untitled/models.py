from datetime import datetime
from exts import db

#定义用户模型
class User(db.Model):
    __tablename__='user'
    id=db.Column(db.Integer,primary_key=True,autoincrement=True)
    telephone=db.Column(db.String(11),nullable=False)
    username=db.Column(db.String(50),nullable=False)
    password=db.Column(db.String(100),nullable=False)

#定义微博模型
class Weibo(db.Model):
    __tablename__='weibo'
    id=db.Column(db.Integer,primary_key=True,autoincrement=True)
    # title=db.Column(db.String(100),nullable=False)
    content=db.Column(db.Text,nullable=False)
    # now()获取的是服务器第一次运行的时间
    # now就是每次创建一个模型的时候都获取当前时间
    create_time=db.Column(db.DateTime,default=datetime.now)
    author_id=db.Column(db.Integer,db.ForeignKey('user.id'))
    #可以使用author.weibos得出当前作者的所有微博
    author=db.relationship('User',backref=db.backref('weibos'))

#定义评论模型
class Answer(db.Model):
    __tablename__='answer'
    id=db.Column(db.Integer,primary_key=True,autoincrement=True)
    content=db.Column(db.Text,nullable=False)
    create_time=db.Column(db.DateTime,default=datetime.now)
    weibo_id=db.Column(db.Integer,db.ForeignKey('weibo.id'))
    author_id=db.Column(db.Integer,db.ForeignKey('user.id'))
    #可以使用weibo.answers得出当前微博的所有评论
    weibo=db.relationship('Weibo',backref=db.backref('answers'))
    #可以使用author.answers得出当前作者的所有评论
    author=db.relationship('User',backref=db.backref('answers'))
