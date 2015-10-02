def addVectors(ax, az, bx, bz):
	return ax + bx, az + bz

def mulVectorByScalar(ax, az, s):
	return ax * s, az * s

def getVectorLength(ax, az):
	return math.sqrt(ax * ax + az * az)
