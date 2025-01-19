import os
import time
import json
import argparse
import re
import shutil
import copy
import pathlib

read_speed_threshold = 120.0  # 120mb/s
read_test_block_size = 4.0  # mb
min_test_file_size = 4.0  # mb
min_refresh_interval = 90  # days
sleep_duration_factor = 0.001  # mb -> s

database_file = "ssd_refresh.json"

black_list = [r".*\$recycle.bin.*", r".*ssd_refresh.*"]


def from_megabytes(num_megabytes: float) -> int:
    return int(num_megabytes * 1024 * 1024)


def to_megabytes(num_bytes: int) -> float:
    return num_bytes / 1024 / 1024


def to_days(num_seconds: float) -> float:
    return num_seconds / 24 / 3600


def file_need_refresh(path: str) -> bool:
    if not os.path.exists(path):
        return False
    file_size = os.path.getsize(path)
    if file_size == 0:
        return False
    if to_megabytes(file_size) < min_test_file_size:
        return True
    num_bytes_to_read = min(from_megabytes(read_test_block_size), file_size)
    start_time = time.time()
    open(path, "rb").read(num_bytes_to_read)
    duration = time.time() - start_time
    if duration == 0.0:
        return False
    read_speed = to_megabytes(num_bytes_to_read) / duration
    return read_speed < read_speed_threshold


def normalized_path(path: str) -> str:
    path = os.path.normpath(path)
    path = os.path.normcase(path)
    return path


def try_refresh_file(path: str) -> None:
    if not os.path.exists(path):
        return
    path = str(pathlib.WindowsPath(path).resolve())
    clone_path = path + ".sr.clone"
    backup_path = path + ".sr.bak"
    try:
        shutil.copy2(path, clone_path)
        if os.path.exists(clone_path) and os.path.getsize(path) == os.path.getsize(
            clone_path
        ):
            shutil.move(path, backup_path)
            shutil.move(clone_path, path)
            os.remove(backup_path)
    except KeyboardInterrupt:

        def try_remove(path: str):
            if os.path.exists(path):
                os.remove(path)

        if not os.path.exists(path):
            if os.path.exists(backup_path):
                shutil.move(backup_path, path)
            elif os.path.exists(clone_path):
                shutil.move(clone_path, path)
            else:
                print("[WARNING] file missing: {}")
        try_remove(clone_path)
        try_remove(backup_path)
        raise
    return


def is_path_in_blacklist(normalized_path: str) -> bool:
    return any(map(lambda p: re.match(p, normalized_path) != None, black_list))


class SSDRefresh:
    def __init__(self) -> None:
        self.files: dict[str, float] = {}
        self.__try_read_database()

    def __try_read_database(self) -> None:
        if os.path.exists(database_file):
            root = json.load(open(database_file, encoding="utf-8"))
            self.files = root["files"]

    def save(self) -> None:
        try:
            if os.path.exists(database_file):
                shutil.copy2(database_file, database_file + ".bak")
            with open(database_file, "w", encoding="utf-8") as w:
                to_save = {"files": self.files}
                json.dump(to_save, w, indent=4)
        except:
            pass

    def walk(self, root: str) -> None:
        cur_files: set[str] = set()
        for dirpath, _, filenames in os.walk(root):
            for filename in filenames:
                path = normalized_path(os.path.join(dirpath, filename))
                if not is_path_in_blacklist(path):
                    cur_files.add(path)

        for cur_file in cur_files:
            if cur_file not in self.files:
                self.files[cur_file] = 0
        return

    def run(self) -> None:
        items = sorted(copy.deepcopy(self.files).items())
        removed_files = []
        for path, last_refresh_time in items:
            if not os.path.exists(path) or is_path_in_blacklist(path):
                print(f"skipping (not existing/blacklist): {path}")
                removed_files.append(path)
                continue
            if to_days(time.time() - last_refresh_time) < min_refresh_interval:
                print(f"skipping (refresh time): {path}")
                continue
            if not file_need_refresh(path):
                print(f"skipping (read speed): {path}")
                continue
            print(f"refreshing: {path}")
            try_refresh_file(path)
            self.files[path] = time.time()
            time.sleep(sleep_duration_factor * to_megabytes(os.path.getsize(path)))
        for path in removed_files:
            self.files.pop(path)
        return

    def dump(self) -> None:
        items = sorted(self.files.items())
        for path, last_refresh_time in items:
            time_str = time.asctime(time.localtime(last_refresh_time))
            print(f"{path}: {time_str}")
        return


def main():
    parser = argparse.ArgumentParser()
    method = parser.add_mutually_exclusive_group(required=True)
    method.add_argument("-w", "--walk", type=str)
    method.add_argument("-r", "--run", action="store_true")
    method.add_argument("-d", "--dump", action="store_true")
    args = parser.parse_args()

    refresh = SSDRefresh()

    try:
        if args.walk is not None:
            refresh.walk(args.walk)
        elif args.run is not None and args.run:
            refresh.run()
        elif args.dump is not None and args.dump:
            refresh.dump()
        refresh.save()
    except:
        refresh.save()
        raise


if __name__ == "__main__":
    main()
