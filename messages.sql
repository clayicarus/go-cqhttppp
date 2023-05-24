create table messages
(
    id integer primary key autoincrement,
    group_id integer,
    sender_id integer,
    msg_time integer not null,
    context text not null
);
