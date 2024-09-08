import winreg

FONT_LINK_REG_PATH = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\FontLink\\SystemLink"
CHS_FONT_KEYWORDS = [
    "YaHei",
    "SimSun"
]
OTHER_FONT_KEYWORDS = [
    # Korean
    "Batang",
    "Gulim",
    "Malgun",

    # Japanese
    "Meiryo",
    "MS Gothic",
    "MS UI Gothic",
    "MS Mincho",
    "Yu Gothic",

    # Traditional Chinese
    "MingLiU",
    "JhengHei"
]
VALID_EN_FONT_KEYWORDS = [
    "Ebrima",
    "Lucida Sans",
    "Microsoft Sans Serif",
    "Segoe",
    "Tahoma"
]

def str_contains(string: str, substring: str) -> bool:
    return string.lower().find(substring.lower()) != -1

def sort_font_fallback_list(font_list: list[str]) -> list[str] | None:
    chs_items = []
    for item in font_list:
        if any(map(lambda keyword: str_contains(item, keyword), CHS_FONT_KEYWORDS)):
            chs_items.append(item)
    if len(chs_items) == 0:
        return None
    for item in chs_items:
        font_list.remove(item)

    insertion_point = -1
    for i in range(len(font_list)):
        should_insert = False
        if any(map(lambda keyword: str_contains(font_list[i], keyword), OTHER_FONT_KEYWORDS)):
            should_insert = True
            insertion_point = i
            break
        if should_insert:
            break
    
    if insertion_point == -1:
        return None

    for i in range(len(chs_items)):
        font_list.insert(insertion_point + i, chs_items[i])
    return font_list

def main():
    font_link_key = winreg.OpenKeyEx(winreg.HKEY_LOCAL_MACHINE, FONT_LINK_REG_PATH, 0, winreg.KEY_READ | winreg.KEY_SET_VALUE)
    _, num_items, _ = winreg.QueryInfoKey(font_link_key)

    for i in range(num_items):
        name, font_list, type = winreg.EnumValue(font_link_key, i)
        assert type == 7
        if not any(map(lambda keyword: str_contains(name, keyword), VALID_EN_FONT_KEYWORDS)):
            continue
        sorted_font_list = sort_font_fallback_list(font_list)
        if sorted_font_list is None:
            continue
        winreg.SetValueEx(font_link_key, name, 0, type, sorted_font_list)
    return

if __name__=="__main__":
    main()
