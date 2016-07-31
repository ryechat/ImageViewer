#pragma once
#ifndef CUnitDispenser_h
#define CUnitDispenser_h

#ifndef STDAFX_H
#endif


// ���l�����̒P�ʂɒB�����1�܂���-1���������A�]�蕪�͕ۑ�����
// �}�E�X�z�C�[����J�[�\���̈ړ��ʂ��A�񐔕��̃R�}���h�ɕϊ�����Ƃ��g��
template<class T>
class CUnitDispenser {
private:
	T value;
	T unit;

public:
	CUnitDispenser() : value(0), unit(0) {};

	CUnitDispenser(const T& Value, const T& Unit) {
		value = Value;
		unit = Unit;
	}

	~CUnitDispenser() = default;

	void setUnit(const T& newUnit) { unit = newUnit; }

	void reset(const T& newValue) { value = newValue; }

	T get() {
		if (value >= unit) {
			value -= unit;
			return unit;
		}
		else if (value <= -unit) {
			value += unit;
			return -unit;
		}
		return{};
	}

	bool add(const T& addition)	{
		value += addition;
		return (over() != 0);
	}

	bool over() {
		return value >= unit || value <= -unit;
	}

};

#endif