import math

## segment = (x1, y) to (x2, y), circle = (xc, yc) and radius r
## if x1 <= xc <= x2
##	if abs(y - yc) <= r
		# return true
##	else:
##		if ((x1 - xc)^2 + (y - yc)^2 <= r^2) or ((x2 - xc)^2 + (y - yc)^2 <= r^2) return true

def qsqr(x):
	return x*x

# x1 is the left x of the horizontal segment, x2 is the right x.
def collideCircleVsHSegment(x1, x2, y, xc, yc, r):
	if x1 <= xc <= x2:
		if abs(y - yc) <= r:
			return True
		else:
			r2 = qsqr(r)
			if (qsqr(x1 - xc) + qsqr(y - yc) <= r2) or (qsqr(x2 - xc) + qsqr(y - yc) <= r2):
				return True

	return False

def collideCircleVsVSegment(x, y1, y2, xc, yc, r):
	if y1 <= yc <= y2:
		if abs(x - yc) <= r:
			return True
		else:
			r2 = qsqr(r)
			if (qsqr(y1 - yc) + qsqr(x - xc) <= r2) or (qsqr(y2 - yc) + qsqr(x - xc) <= r2):
				return True

	return False
