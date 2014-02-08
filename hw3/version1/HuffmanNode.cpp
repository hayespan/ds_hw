#include "HuffmanNode.h"
HuffmanNode::HuffmanNode(char ch, int freq, HuffmanNode * left, HuffmanNode * right, HuffmanNode * parent)
	:ch_(ch),freq_(freq),code_(""),left_(left),right_(right),parent_(parent)
{}
