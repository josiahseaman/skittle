from django import template

register = template.Library()


@register.filter
def toSI(value):
    SImulitplePrefixes = ['', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y']
    SIfractionPrefixes = ['m', unichr(181), 'n', 'p', 'f', 'a', 'z', 'y']

    value = float(value)
    if value >= 1 or value <= -1:
        for i in range(0, 8):
            if 1000 > value > -1000:
                if 10 > value > 0 or -10 < value < 0:
                    value = str(round(value, 2))
                elif 100 > value > 0 or -100 < value < 0:
                    value = str(round(value, 1))
                else:
                    value = str(int(value))
                return value + ' ' + SImulitplePrefixes[i]
            else:
                value /= 1000
    else:
        for i in range(0, 7):
            value *= 1000
            if value > 1 or value < -1:
                if 10 > value > 0 or -10 < value < 0:
                    value = str(round(value, 2))
                elif 100 > value > 0 or -100 < value < 0:
                    value = str(round(value, 1))
                else:
                    value = str(int(value))
                return value + ' ' + SIfractionPrefixes[i]

@register.filter
def keyvalue(dict, key):    
    return dict[key]