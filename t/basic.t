#!/usr/bin/perl

# Tests for request and upstream variables from http extra variables.

###############################################################################

use warnings;
use strict;

use Test::More;

use lib 'lib';
use Test::Nginx qw/ :DEFAULT /;

###############################################################################

select STDERR; $| = 1;
select STDOUT; $| = 1;

my $t = Test::Nginx->new()->has(qw/http proxy ngx_http_extra_variables_module/)
	->plan(17);

$t->write_file_expand('nginx.conf', <<'EOF');

%%TEST_GLOBALS%%

daemon off;

events {
}

http {
    %%TEST_GLOBALS_HTTP%%

    server {
        listen       127.0.0.1:8080;
        server_name  localhost;

        location / {
            proxy_pass http://127.0.0.1:8081;

            add_header X-Request-Path $request_path always;
            add_header X-Request-Args $request_args always;
            add_header X-Request-Is-Args $request_is_args always;
            add_header X-Request-Has-Args $request_has_args always;
            add_header X-Request-Arg-Foo $request_arg_foo always;
            add_header X-Current-URI $current_uri always;
            add_header X-Current-Path $current_path always;
            add_header X-Has-Args $has_args always;
            add_header X-Dollar $dollar always;
            add_header X-Upstream-Scheme $upstream_scheme always;
            add_header X-Upstream-URI $upstream_uri always;
            add_header X-Upstream-Status $upstream_last_status always;
            add_header X-Upstream-Tries $upstream_tries always;
            add_header X-Upstream-Addr $upstream_last_addr always;
        }
    }

    server {
        listen       127.0.0.1:8081;
        server_name  localhost;

        location / {
            return 200 upstream;
        }
    }
}

EOF

$t->run();

###############################################################################

my $response = http_get('/hello/world?foo=bar&n=2');

like($response, qr/^HTTP\/1\.1 200 /, 'upstream response status');
like($response, qr/\r?\n\r?\nupstream$/,
	'upstream response body');
is(header($response, 'X-Request-Path'), '/hello/world',
	'request path variable');
is(header($response, 'X-Request-Args'), 'foo=bar&n=2',
	'request args variable');
is(header($response, 'X-Request-Is-Args'), '?',
	'request is-args variable');
is(header($response, 'X-Request-Has-Args'), '&',
	'request has-args variable');
is(header($response, 'X-Request-Arg-Foo'), 'bar',
	'request argument variable');
is(header($response, 'X-Current-URI'), '/hello/world?foo=bar&n=2',
	'current URI variable');
is(header($response, 'X-Current-Path'), '/hello/world',
	'current path variable');
is(header($response, 'X-Has-Args'), '&', 'has-args variable');
is(header($response, 'X-Dollar'), '$', 'dollar variable');
is(header($response, 'X-Upstream-Scheme'), 'http',
	'upstream scheme variable');
like(header($response, 'X-Upstream-URI'), qr{^/hello/world},
	'upstream URI variable');
is(header($response, 'X-Upstream-Status'), '200',
	'upstream status variable');
is(header($response, 'X-Upstream-Tries'), '1',
	'upstream tries variable');
like(header($response, 'X-Upstream-Addr'), qr/^127\.0\.0\.1:8081$/,
	'upstream address variable');

my $plain = http_get('/plain');
is(header($plain, 'X-Request-Has-Args'), '?',
	'request has-args variable without arguments');

###############################################################################

sub header {
	my ($response, $name) = @_;

	return $1 if $response =~ /^\Q$name\E: ([^\r\n]*)\x0d?$/mi;
	die "no $name header in response: $response";
}
