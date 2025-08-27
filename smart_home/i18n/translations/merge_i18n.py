import yaml
from pathlib import Path

def load_language_data(file_path, lang_key):
    """加载YAML文件并提取指定语言键下的数据"""
    try:
        if not Path(file_path).exists():
            raise FileNotFoundError(f"文件不存在: {file_path}")
        
        with open(file_path, 'r', encoding='utf-8') as f:
            data = yaml.safe_load(f) or {}  # 处理空文件
        
        return data.get(lang_key, {})  # 提取语言键下的内容（无则返回空字典）
    
    except yaml.YAMLError as e:
        raise ValueError(f"YAML解析失败: {str(e)}") from e

def merge_and_sort_yml(zh_path, en_path, jp_path, ru_path, ar_path, output_path, merge_all_keys=True):
    """合并中英文YAML数据并按key首字母排序"""
    try:
        # 加载中英文数据
        zh_data = load_language_data(zh_path, 'zh-CN')
        en_data = load_language_data(en_path, 'en-US')
        jp_data = load_language_data(jp_path, 'ja-JP')
        ru_data = load_language_data(ru_path, 'ru-RU')
        ar_data = load_language_data(ar_path, 'ru-RU')
        
        # 确定需要处理的键集合
        if merge_all_keys:
            keys_to_process = zh_data.keys() | en_data.keys()  # 所有键（并集）
        else:
            keys_to_process = zh_data.keys() & en_data.keys()  # 共同键（交集）
        
        # 按首字母顺序排序键（区分大小写，可根据需要调整）
        sorted_keys = sorted(keys_to_process)
        
        # 构建合并数据（按排序后的键顺序）
        merged_data = []
        for key in sorted_keys:
            merged_item = {
                'code': key,
                'ch': zh_data.get(key, ''),  # 中文无此键则留空
                'en': en_data.get(key, "*Not translated*"),
                'jp': jp_data.get(key, "*Not Support*"),
                'ru': ru_data.get(key, "*Not Support*"),
                'ar': ar_data.get(key, "*Not Support*")  
            }
            merged_data.append(merged_item)
        
        # 写入输出文件（YAML格式）
        with open(output_path, 'w', encoding='utf-8') as f:
            yaml.dump(
                merged_data,
                f,
                allow_unicode=True,  # 支持中文
                sort_keys=False,     # 保持我们手动排序的顺序（不自动排序）
                indent=2             # 缩进2空格，更易读
            )
        
        print(f"合并完成！已按首字母排序，结果保存至: {output_path}")
    
    except Exception as e:
        print(f"合并失败: {str(e)}")

if __name__ == "__main__":
    # 配置路径（根据实际情况修改）
    ZH_FILE = "zh-CN.yml"    # 中文YAML文件路径
    EN_FILE = "en-US.yml"    # 英文YAML文件路径
    JP_FILE = "ja-JP.yml"    # 英文YAML文件路径
    RU_FILE = "ru-RU.yml"    # 英文YAML文件路径
    AR_FILE = "ar.yml"    # 英文YAML文件路径
    OUTPUT_FILE = "pro-i18n.yaml"   # 输出文件路径
    
    # 执行合并（merge_all_keys=True: 合并所有键；False: 仅共同键）
    merge_and_sort_yml(
        zh_path=ZH_FILE,
        en_path=EN_FILE,
        jp_path=JP_FILE,
        ru_path=RU_FILE,
        ar_path=AR_FILE,
        output_path=OUTPUT_FILE,
        merge_all_keys=True
    )
    