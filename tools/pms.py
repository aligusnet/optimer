#!/usr/bin/python
#
# Copyright (c) 2013 Alexander Ignatyev. All rights reserved.
#
# Set parameters in function main()

import os
import sys
import time
import datetime
import subprocess
from copy import copy
from tempfile import NamedTemporaryFile

class Timer(object):
    def __init__(self):
        if sys.platform == "win32":
            self.timer = time.clock
        else:
            self.timer = time.time
        self.reset()

    def reset(self):
        self.start_time = self.timer()

    def elapsed(self):
        return self.timer() - self.start_time

class RunningResult(object):
    def __init__(self, process):
        timer = Timer()
        self.stdout, self.stderr = process.communicate()
        self.running_time = timer.elapsed()
        self.return_code = process.returncode

    def __str__(self):
        return 'return: {0}, total time: {1}'.format(self.return_code, self.running_time)

    def __repr__(self):
        return '<<return: {0}, total time: {1}>>'.format(self.return_code, self.running_time)

def run_task(args):
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return RunningResult(process)

class ConfigParams(object):
    default_valuations = ['serial', 'parallel-lock']
    default_containers = ['lifo', 'priority']
    default_schedulers = ['giving', 'requesting']
    default_num_threads = [4]
    default_num_minimum_nodes = [8]
    default_num_maximum_nodes = [80]
    
    def __init__(self
        , valuations = None
        , containers = None
        , schedulers = None
        , num_threads = None
        , num_minimum_nodes = None
        , num_maximum_nodes = None):
        if valuations:
            self.valuations = valuations
        else:
            self.valuations = ConfigParams.default_valuations

        if containers:
            self.containers = containers
        else:
            self.containers = copy(ConfigParams.default_containers)

        if schedulers:
            self.schedulers = schedulers
        else:
            self.schedulers = copy(ConfigParams.default_schedulers)

        if num_threads:
            self.num_threads = num_threads
        else:
            self.num_threads = copy(ConfigParams.default_num_threads)

        if num_minimum_nodes:
            self.num_minimum_nodes = num_minimum_nodes
        else:
            self.num_minimum_nodes = copy(ConfigParams.default_num_minimum_nodes)

        if num_maximum_nodes:
            self.num_maximum_nodes = num_maximum_nodes
        else:
            self.num_maximum_nodes = copy(ConfigParams.default_num_maximum_nodes)

class Config(object):
    def generate_tasks(self, params, problem_path):
        self.params = params
        general = {}
        general['problem_path'] = problem_path

        config = {}
        config['general'] = general

        self.config_files = {}

        for valuation in self.params.valuations:
            if valuation == 'serial':
                self.__generate_serial_tasks(config)
            elif valuation == 'parallel-lock':
                self.__generate_parallel_lock_tasks(config)

        return self.config_files

    def __generate_serial_tasks(self, config):
        general = config['general']
        general['valuation_type'] = 'serial'
        for container in self.params.containers:
            general['container_type'] = container
            name = Config.__build_name(config)
            self.config_files[name] = Config.__save_config(config)

    def __generate_parallel_lock_tasks(self, config):
        general = config['general']
        general['valuation_type'] = 'parallel-lock'
        scheduler = {}
        config['scheduler'] = scheduler
        for container in self.params.containers:
            general['container_type'] = container
            for scheduler_type in self.params.schedulers:
                scheduler['type'] = scheduler_type
                for num_threads in self.params.num_threads:
                    scheduler['num_threads'] = num_threads
                    for num_minimum_nodes in self.params.num_minimum_nodes:
                        scheduler['num_minimum_nodes'] = num_minimum_nodes
                        if scheduler_type == 'giving':
                            for num_maximum_nodes in self.params.num_maximum_nodes:
                                scheduler['num_maximum_nodes'] = num_maximum_nodes
                                name = Config.__build_name(config)
                                self.config_files[name] = Config.__save_config(config)
                        else:
                            name = Config.__build_name(config)
                            self.config_files[name] = Config.__save_config(config)

    @staticmethod
    def __build_name(config):
        general = config['general']
        parts = []
        parts.append(os.path.basename(general['problem_path']))
        parts.append(general['valuation_type'])
        parts.append(general['container_type'])
        if general['valuation_type'] == 'parallel-lock':
            scheduler = config['scheduler']
            parts.append(scheduler['type'])
            parts.append('{0: 3}'.format(scheduler['num_threads']))
            parts.append('{0: 3}'.format(scheduler['num_minimum_nodes']))
            if scheduler['type'] == 'giving':
                parts.append('{0: 3}'.format(scheduler['num_maximum_nodes']))
        return ':'.join(parts)

    @staticmethod
    def __save_config(config):
        lines = []
        for section_name, section in config.iteritems():
            lines.append('[{0}]\n'.format(section_name))
            for key, value in section.iteritems():
                lines.append('{0} = {1}\n'.format(key, value))
        with NamedTemporaryFile(delete=False) as file:
            filename = file.name
            file.writelines(lines)
        return filename

def main():
    # set params section
    module = 'build_clang++_release/atsp'
    problem_path = 'data/ftv38.atsp'
    num_runs = 2

    params = ConfigParams()
    params.valuations = ['serial', 'parallel-lock']
    params.schedulers = ['requesting']
    params.num_threads = [2, 4, 8]
    params.num_minimum_nodes = [4, 8]
    params.num_maximum_nodes = [10, 20, 40, 80]
    # end set params section

    print 'generating tasks...'
    config = Config()
    config_files = config.generate_tasks(params, problem_path)

    num = 0
    num_configs = len(config_files)
    all_runs = {}
    for task_name in sorted(config_files.iterkeys()):
        num += 1
        filename = config_files[task_name]
        print '[{0: 3}/{1}] '.format(num, num_configs), 'running ', task_name, '...'
        runs = []
        for _ in xrange(num_runs):
            res = run_task([module, filename])
            runs.append(res)
            if res.return_code != 0:
                print 'stdout: ['+res.stdout+']'
                print 'stderr: ['+res.stderr+']'
        all_runs[task_name] = runs

    print 'preparing results...'
    all_avg_times = {}
    for task_name, runs in all_runs.iteritems():
        avg_time = reduce(lambda sum, run: sum + run.running_time, runs, 0.0) / len(runs)
        all_avg_times[task_name] = avg_time

    result_filename = 'pms_result.'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S')+'.txt'
    print 'saving results to', result_filename, '...'
    lines = []
    lines.append('Average results:\n')
    for task_name in sorted(all_avg_times.iterkeys()):
        lines.append('{0} {1}\n'.format(task_name, all_avg_times[task_name]))

    lines.append('\nResults:\n')
    for task_name in sorted(all_runs.iterkeys()):
        lines.append('{0} {1}\n'.format(task_name, all_runs[task_name]))

    with open(result_filename, 'w') as f:
        f.writelines(lines)

    print 'done'

if __name__ == '__main__':
    main()