#include <string>
#include "Orders.h"
#include "Order.h"

using std::string;

Order MyOrder::COMBO_LIMIT_ORDER(string action, Decimal quantity, double limitPrice, bool nonGuaranteed){
	// ! [combolimit]
	Order order;
	order.action = action;
	order.orderType = "LMT";
	order.totalQuantity = quantity;
	order.lmtPrice = limitPrice;
	if(nonGuaranteed){
		TagValueSPtr tag1(new TagValue("NonGuaranteed", "1"));
		order.smartComboRoutingParams.reset(new TagValueList());
		order.smartComboRoutingParams->push_back(tag1);
	}
	return order;
}

Order MyOrder::MARKET_ORDER(string action, Decimal quantity){
	Order order;
	order.action = action;
	order.orderType = "MKT";
	order.totalQuantity = quantity;
	return order;
}

Order MyOrder::LIMIT_ORDER(string action, Decimal quantity, double limitPrice){
	Order order;
	order.action = action;
	order.orderType = "LMT";
	order.totalQuantity = quantity;
	order.lmtPrice = limitPrice;
	return order;
}