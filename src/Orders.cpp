#include "Orders.h"
#include "Order.h"

Order MyOrder::COMBOLIMITORDER(std::string action, Decimal quantity, double limitPrice, bool nonGuaranteed){
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