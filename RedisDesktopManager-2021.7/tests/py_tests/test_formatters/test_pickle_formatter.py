from datetime import datetime
import json
import pickle
import unittest

from ddt import ddt, data
import numpy as np
import pandas as pd


from src.py.formatters.pickle import PickleFormatter


@ddt
class TestPickleFormatter(unittest.TestCase):
    formatter = PickleFormatter()

    @data(
        [1, 2, 3, 4],
        datetime.now(),
        np.array([[1, 2], [3, 4]]),
        np.array([1, 2, 3], dtype=complex),
        pd.Series(np.random.randn(5), index=['a', 'b', 'c', 'd', 'e']),
        pd.Series({'b': 1, 'a': 0, 'c': 2}),
        pd.Series({'b': 1, 'a': 0, 'c': 2}, index=['b', 'c', 'd', 'a']),
        pd.Series(np.random.randn(5)),
        pd.Series(pd.date_range('20130101', periods=6)),
        pd.DataFrame(np.random.randn(6, 4),
                     index=pd.date_range('20130101', periods=6),
                     columns=list('ABCD')),
    )
    def test_decode(self, val):
        if isinstance(val, pd.Series):
            expected_output = f'{str(type(val))[1:-1]}\n{val.to_string()}'
        elif isinstance(val, pd.DataFrame):
            html = val.to_html(render_links=True, border=0)
            expected_output = self.formatter.format_html_output(val, html)
        elif isinstance(val, np.ndarray):
            html = pd.DataFrame(val).to_html(render_links=True, border=0)
            expected_output = self.formatter.format_html_output(val, html)
        else:
            expected_output = json.dumps(val, default=self.formatter.default,
                                         ensure_ascii=False)

        pickled_val = pickle.dumps(val)
        formatter_response_dict = self.formatter.decode(pickled_val)

        self.assertIn('output', formatter_response_dict)

        actual_output = formatter_response_dict['output']
        self.assertEqual(actual_output, expected_output)
