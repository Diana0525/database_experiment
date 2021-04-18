/*==============================================================*/
/* DBMS name:      MySQL 5.0                                    */
/* Created on:     2021/4/3 17:58:05                            */
/*==============================================================*/


drop  
table  if   exists   message_detail;

/*==============================================================*/
/* Table: article                                               */
/*==============================================================*/
create table article
(
   article_ID           int not null auto_increment,
   message_ID           int not null,
   section_ID           int not null,
   t_article_ID         int,
   article              text not null,
   article_type         int not null,
   primary key (article_ID)
);

/*==============================================================*/
/* Table: comments                                              */
/*==============================================================*/
create table comments
(
   comment_ID           int not null auto_increment,
   message_ID           int not null,
   comment_text         text not null,
   primary key (comment_ID)
);

/*==============================================================*/
/* Table: follow                                                */
/*==============================================================*/
create table follow
(
   user_ID              int not null,
   use_user_ID          int not null,
   group_ID             int,
   primary key (user_ID, use_user_ID)
);

/*==============================================================*/
/* Table: group_user                                            */
/*==============================================================*/
create table group_user
(
   group_ID             int not null auto_increment,
   user_ID              int not null,
   group_name           char(10) not null,
   primary key (group_ID)
);

/*==============================================================*/
/* Table: message1                                              */
/*==============================================================*/
create table message1
(
   message_ID           int not null auto_increment,
   user_ID              int not null,
   message_time         datetime not null,
   valid                bool not null,
   primary key (message_ID)
);

/*==============================================================*/
/* Index: time_index_key                                        */
/*==============================================================*/
create index time_index_key on message1
(
   message_time
);

/*==============================================================*/
/* Table: praise                                                */
/*==============================================================*/
create table praise
(
   user_ID              int not null,
   message_ID           int not null,
   praise_time          datetime,
   primary key (user_ID, message_ID)
);

/*==============================================================*/
/* Table: reply                                                 */
/*==============================================================*/
create table reply
(
   reply_ID             int not null auto_increment,
   comment_ID           int not null,
   reply                text not null,
   reply_time           datetime not null,
   primary key (reply_ID)
);

/*==============================================================*/
/* Table: section                                               */
/*==============================================================*/
create table section
(
   section_ID           int not null auto_increment,
   section_name         char(20) not null,
   primary key (section_ID)
);

/*==============================================================*/
/* Table: user_detail                                           */
/*==============================================================*/
create table user_detail
(
   detail_ID            int not null auto_increment,
   user_ID              int not null,
   sex                  char(5),
   education            char(10),
   job                  char(20),
   address              char(20),
   individual_resume    text,
   phone                int,
   mailbox              char(30),
   primary key (detial_ID)
);

/*==============================================================*/
/* Table: userinfo                                              */
/*==============================================================*/
create table userinfo
(
   user_ID              int not null auto_increment,
   user_name            char(10) not null,
   user_psw             varchar(20) not null,
   user_state           bool not null,
   primary key (user_ID)
);

/*==============================================================*/
/* Table: 艾特                                                    */
/*==============================================================*/
create table 艾特
(
   message_ID           int not null,
   user_ID              int not null,
   at_ID                int not null,
   primary key (at_ID)
);

/*==============================================================*/
/* View: message_detail                                         */
/*==============================================================*/
create  VIEW      message_detail
  as
select m1.user_ID,m1.message_time,m1.valid,
    a.section_ID,a.article,a.article_type,a.t_article_ID,
    u.user_name,c.comment_text,num_praise.praise_num,
    num_comments.comment_num
from message1 m1,
    article a,
    comments c,
    userinfo u,
    (select 
        praise.message_id as message_id,
        count(praise.user_ID) as praise_num
    from praise
    group by praise.message_id) as num_praise,
    (select comments.message_id as message_id,
            count(comments.comment_ID) as comment_num
    from comments
    group by comments.message_id) as num_comments
where m1.message_ID = c.message_ID
and m1.message_ID = a.message_ID
and m1.user_ID = u.user_ID;

alter table article add constraint FK_文章_板块 foreign key (section_ID)
      references section (section_ID) on delete restrict on update restrict;

alter table article add constraint FK_消息_文章 foreign key (message_ID)
      references message1 (message_ID) on delete restrict on update restrict;

alter table comments add constraint FK_消息_评论 foreign key (message_ID)
      references message1 (message_ID) on delete restrict on update restrict;

alter table follow add constraint FK_关注 foreign key (user_ID)
      references userinfo (user_ID) on delete restrict on update restrict;

alter table follow add constraint FK_被关注 foreign key (use_user_ID)
      references userinfo (user_ID) on delete restrict on update restrict;

alter table group_user add constraint FK_用户_分组 foreign key (user_ID)
      references userinfo (user_ID) on delete restrict on update restrict;

alter table message1 add constraint FK_用户发布消息 foreign key (user_ID)
      references userinfo (user_ID) on delete restrict on update restrict;

alter table praise add constraint FK_点赞 foreign key (message_ID)
      references message1 (message_ID) on delete restrict on update restrict;

alter table praise add constraint FK_点赞2 foreign key (user_ID)
      references userinfo (user_ID) on delete restrict on update restrict;

alter table reply add constraint FK_评论_回复 foreign key (comment_ID)
      references comments (comment_ID) on delete restrict on update restrict;

alter table user_detail add constraint FK_用户信息 foreign key (user_ID)
      references userinfo (user_ID) on delete restrict on update restrict;

alter table 艾特 add constraint FK_艾特 foreign key (message_ID)
      references message1 (message_ID) on delete restrict on update restrict;

alter table 艾特 add constraint FK_艾特2 foreign key (user_ID)
      references userinfo (user_ID) on delete restrict on update restrict;

