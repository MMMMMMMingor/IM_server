#ifndef HANDLER_H
#define HANDLER_H

#include "common.h"
#include "common_util.hpp"
#include "loguru.hpp"
#include "message.pb.h"

void check_login_handler(Context &ctx, im_message::Message &in_message);
void client_login_handler(Context &ctx, im_message::Message &in_message);
void client_logout_handler(Context &ctx, im_message::Message &in_message);
void create_connection_handler(Context &ctx, im_message::Message &in_message);
void in_bound_handler(Context &ctx, im_message::Message &in_message);
void keepalive_handler(Context &ctx, im_message::Message &in_message);
void transmit_message_handler(Context &ctx, im_message::Message &in_message);

#endif