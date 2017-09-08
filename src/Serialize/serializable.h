
/* 
 * File:   serializable.h
 * Author: hamed
 *
 * Created on September 7, 2017, 3:39 PM
 */

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class Serializable{
	virtual void serialize(Serializer* ) = 0;
};

#endif /* SERIALIZABLE_H */

