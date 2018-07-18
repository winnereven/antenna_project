/**
 * File name: IOnDegreeChangedListener.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017年6月20日 下午10:52:36
 * Description: 
 */

#ifndef IONDEGREECHANGEDLISTENER_H_
#define IONDEGREECHANGEDLISTENER_H_

class IOnDegreeChangedListener {
public:
	IOnDegreeChangedListener() {
	}
	virtual ~IOnDegreeChangedListener() {
	}
	virtual void onDegreeChanged(int degree) = 0;
	virtual void onDegreeChangeFinished() = 0;
};

#endif /* IONDEGREECHANGEDLISTENER_H_ */
